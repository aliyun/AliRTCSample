# timer_thread.py
import threading
import time
from typing import Callable, Any, Optional

class PeriodicTimer:
    """
    A class that calls a function periodically in a separate thread.
    """

    def __init__(self, interval: float, function: Callable[..., Any], *args, **kwargs):
        """
        Initialize the periodic timer.

        Args:
            interval: Time interval in seconds between function calls
            function: Function to call periodically
            *args: Positional arguments to pass to the function
            **kwargs: Keyword arguments to pass to the function
        """
        self.interval = interval
        self.function = function
        self.args = args
        self.kwargs = kwargs

        self.timer_thread = None
        self.stop_event = threading.Event()
        self.is_running = False
        self.lock = threading.Lock()

    def start(self) -> bool:
        """
        Start the periodic timer thread.

        Returns:
            bool: True if started successfully, False if already running
        """
        with self.lock:
            if self.is_running:
                print("Timer is already running")
                return False

            self.stop_event.clear()
            self.timer_thread = threading.Thread(target=self._run, daemon=True)
            self.is_running = True
            self.timer_thread.start()
            print(f"Periodic timer started with interval {self.interval} seconds")
            return True

    def stop(self) -> bool:
        """
        Stop the periodic timer thread.

        Returns:
            bool: True if stopped successfully, False if not running
        """
        with self.lock:
            if not self.is_running:
                print("Timer is not running")
                return False

            self.stop_event.set()

            if self.timer_thread and self.timer_thread.is_alive():
                self.timer_thread.join(timeout=self.interval + 1.0)

            self.is_running = False
            print("Periodic timer stopped")
            return True

    def is_active(self) -> bool:
        """
        Check if the timer is currently active.

        Returns:
            bool: True if timer is active, False otherwise
        """
        with self.lock:
            return self.is_running

    def _run(self):
        """
        Internal method that runs the periodic timer.
        """
        try:
            while not self.stop_event.is_set():
                # Call the function
                try:
                    self.function(*self.args, **self.kwargs)
                except Exception as e:
                    print(f"Error in periodic function: {e}")
                
                # Wait for interval or stop signal
                if self.stop_event.wait(self.interval):
                    break
        except Exception as e:
            print(f"Error in timer thread: {e}")
        finally:
            with self.lock:
                self.is_running = False

# Convenience functions for simple use cases
def call_every(interval: float, function: Callable[..., Any], *args, **kwargs) -> PeriodicTimer:
    """
    Create and start a periodic timer that calls a function at regular intervals.

    Args:
        interval: Time interval in seconds
        function: Function to call
        *args: Positional arguments for the function
        **kwargs: Keyword arguments for the function

    Returns:
        PeriodicTimer: The timer instance
    """
    timer = PeriodicTimer(interval, function, *args, **kwargs)
    timer.start()
    return timer
