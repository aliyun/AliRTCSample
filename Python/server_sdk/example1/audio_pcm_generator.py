import threading
import time
import wave

def pcm_frame_reader_precise(pcm_file_path, sample_rate, bits_per_sample, channels, process_function, stop_event=None, rewind=True):
    """
    Read PCM file with precise timing using wait_until approach
    若process_function返回False，则结束循环，否则继续读取文件。
    stop_event: 另一个方式停止循环。
    """
    # Calculate frame size (bytes per 20ms)
    bytes_per_sample = bits_per_sample // 8
    samples_per_20ms = int(sample_rate * 0.02)  # 20ms worth of samples
    frame_size_bytes = samples_per_20ms * bytes_per_sample * channels
    frame_duration = 0.02  # 20ms in seconds

    try:
        with open(pcm_file_path, 'rb') as f:
            start_time = time.perf_counter()
            next_frame_time = start_time

            while not (stop_event and stop_event.is_set()):
                # Read one frame of data
                frame_data = f.read(frame_size_bytes)

                # If we've reached end of file
                if len(frame_data) == 0:
                    if rewind:
                        # Rewind to beginning of file
                        f.seek(0)
                        # Reset timing reference
                        start_time = time.perf_counter()
                        next_frame_time = start_time
                        continue
                    else:
                        # Exit if not rewinding
                        break

                # If we have incomplete frame, pad with zeros
                if len(frame_data) < frame_size_bytes:
                    frame_data += b'\x00' * (frame_size_bytes - len(frame_data))

                # Call the processing function with frame data
                if not process_function(frame_data):
                    break

                # Schedule next frame
                next_frame_time += frame_duration
                current_time = time.perf_counter()

                # Sleep only if needed
                sleep_time = next_frame_time - current_time
                if sleep_time > 0:
                    time.sleep(sleep_time)
                elif sleep_time < -frame_duration:  # Significantly behind, reset timing
                    next_frame_time = current_time + frame_duration

    except Exception as e:
        print(f"Error reading PCM file: {e}")

class PcmFileReader:
    """
    A class that manages PCM file reading in a separate thread.
    Provides Start() and Stop() methods for controlling the reading process.
    """

    def __init__(self, pcm_file_path, sample_rate, bits_per_sample, channels, process_function):
        """
        Initialize the PCM file reader.

        Args:
            pcm_file_path: Path to the PCM file
            sample_rate: Audio sample rate (e.g., 16000)
            bits_per_sample: Bits per sample (e.g., 16)
            channels: Number of audio channels (e.g., 1 for mono, 2 for stereo)
            process_function: Function to call for each frame
        """
        self.pcm_file_path = pcm_file_path
        self.sample_rate = sample_rate
        self.bits_per_sample = bits_per_sample
        self.channels = channels
        self.process_function = process_function

        self.stop_event = None
        self.reader_thread = None
        self.is_running = False

    def Start(self, rewind=True):
        """
        Start reading PCM file in a separate thread.

        Args:
            rewind: Whether to rewind to beginning when reaching end of file
        """
        if self.is_running:
            print("PCM reader is already running")
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
        print("PCM file reader started")
        return True

    def Stop(self):
        """
        Stop the PCM file reading thread.
        """
        if not self.is_running:
            print("PCM reader is not running")
            return False

        # Signal the thread to stop
        self.stop_event.set()

        # Wait for the thread to finish
        if self.reader_thread and self.reader_thread.is_alive():
            self.reader_thread.join(timeout=5.0)  # Wait up to 5 seconds
            if self.reader_thread.is_alive():
                print("Warning: PCM reader thread did not stop gracefully")

        self.is_running = False
        print("PCM file reader stopped")
        return True

    def _run_reader(self, rewind):
        """
        Internal method that runs the PCM reader in a separate thread.
        """
        try:
            pcm_frame_reader_precise(
                self.pcm_file_path,
                self.sample_rate,
                self.bits_per_sample,
                self.channels,
                self.process_function,
                self.stop_event,
                rewind
            )
        except Exception as e:
            print(f"Error in PCM file reader thread: {e}")
        finally:
            self.is_running = False

    def IsRunning(self):
        """
        Check if the PCM reader is currently running.

        Returns:
            bool: True if running, False otherwise
        """
        return self.is_running

