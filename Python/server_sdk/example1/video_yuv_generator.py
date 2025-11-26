import threading
import time
from typing import Callable, Optional

class YuvFrameReader:
    """
    A module to read YUV files and call a callback for each frame at specified frame rate.
    """

    def __init__(self, yuv_file_path: str, width: int, height: int, 
                 frame_rate: float, process_function: Callable[[bytes], bool]):
        """
        Initialize the YUV frame reader.

        Args:
            yuv_file_path: Path to the YUV file
            width: Video width in pixels
            height: Video height in pixels
            frame_rate: Target frame rate (frames per second)
            process_function: Callback function to process each frame
        """
        self.yuv_file_path = yuv_file_path
        self.width = width
        self.height = height
        self.frame_rate = frame_rate
        self.process_function = process_function

        # Calculate YUV420 frame size
        self.frame_size = width * height * 3 // 2  # YUV420 format

        self.stop_event = None
        self.reader_thread = None
        self.is_running = False
        # 1, 串行化 Start(), Stop()
        # 2, 保护变量不被多个线程同时访问。这里有:
        #    self.is_running
        #    self.stop_event
        #    self.reader_thread
        #    都没有风险。
        self.lock = threading.Lock()

    def Start(self, rewind: bool = True) -> bool:
        """
        Start reading YUV file in a separate thread.

        Args:
            rewind: Whether to rewind to beginning when reaching end of file

        Returns:
            bool: True if started successfully, False otherwise
        """
        with self.lock:
            if self.is_running:
                print("YUV reader is already running")
                return False

            # Create a new stop event for this session
            self.stop_event = threading.Event()

            # Create and start the reader thread
            self.reader_thread = threading.Thread(
                target=self._run_reader,
                args=(rewind,),
                daemon=True  # Thread will die when main program exits
            )

            self.is_running = True
            self.reader_thread.start()
            print(f"YUV file reader started: {self.yuv_file_path}")
            return True

    def Stop(self) -> bool:
        """
        Stop the YUV file reading thread.

        Returns:
            bool: True if stopped successfully, False otherwise
        """
        with self.lock:
            if not self.is_running:
                print("YUV reader is not running")
                return False

            # Signal the thread to stop
            self.stop_event.set()

            # Wait for the thread to finish
            if self.reader_thread and self.reader_thread.is_alive():
                self.reader_thread.join(timeout=5.0)  # Wait up to 5 seconds
                if self.reader_thread.is_alive():
                    print("Warning: YUV reader thread did not stop gracefully")

            # Don't set self.is_running = False here, let _run_reader do it
            print("YUV file reader stopped")
            return True

    def _run_reader(self, rewind: bool):
        """
        Internal method that runs the YUV reader in a separate thread.
        """
        try:
            self._read_yuv_frames(rewind)
        except Exception as e:
            print(f"Error in YUV file reader thread: {e}")
        finally:
            # Just set the flag without acquiring lock to avoid deadlock
            self.is_running = False

    def _read_yuv_frames(self, rewind: bool):
        """
        Read YUV frames and call process function at specified frame rate.
        """
        frame_duration = 1.0 / self.frame_rate if self.frame_rate > 0 else 0

        with open(self.yuv_file_path, 'rb') as f:
            start_time = time.perf_counter()
            frame_count = 0
            next_frame_time = start_time

            while not self.stop_event.is_set():
                # Read one frame of data
                frame_data = f.read(self.frame_size)

                # If we've reached end of file
                if len(frame_data) == 0:
                    if rewind:
                        # Rewind to beginning of file
                        f.seek(0)
                        # Reset timing reference
                        start_time = time.perf_counter()
                        frame_count = 0
                        next_frame_time = start_time
                        continue
                    else:
                        # Exit if not rewinding
                        break

                # If we have incomplete frame, pad with zeros
                if len(frame_data) < self.frame_size:
                    frame_data += b'\x00' * (self.frame_size - len(frame_data))

                # Call the processing function with frame data
                try:
                    should_continue = self.process_function(frame_data)
                    if not should_continue:
                        break
                except Exception as e:
                    print(f"Error in process function: {e}")
                    break

                # Timing control
                frame_count += 1
                if frame_duration > 0:
                    next_frame_time = start_time + (frame_count * frame_duration)
                    current_time = time.perf_counter()
                    sleep_time = next_frame_time - current_time

                    if sleep_time > 0:
                        # Use wait with timeout to allow early stopping
                        if self.stop_event.wait(sleep_time):
                            break
                    elif sleep_time < -frame_duration:  # Significantly behind, reset timing
                        start_time = current_time
                        frame_count = 0

        print("YUV frame reading completed")

