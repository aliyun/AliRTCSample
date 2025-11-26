# audio_wav_recorder.py
import wave
import threading
import os
from datetime import datetime
from collections import defaultdict

class AudioWavRecorder:
    """
    A module to record PCM audio data to WAV files.
    """

    def __init__(self):
        """
        Initialize the WAV recorder.
        """
        self.sample_rate = 48000
        self.channels = 2
        self.bits_per_sample = 16
        self.wav_file = None
        self.is_recording = False
        self.lock = threading.Lock()
        self.file_path = None

    def start_recording(self, file_path=None):
        """
        Start recording to a WAV file.

        Args:
            file_path: Path to the WAV file. If None, generates a timestamp-based filename.
            
        Returns:
            bool: True if recording started successfully, False otherwise
        """
        with self.lock:
            if self.is_recording:
                print("Already recording")
                return False

            # Generate filename if not provided
            if file_path is None:
                timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
                file_path = f"recorded_audio_{timestamp}.wav"

            try:
                # Create directory if it doesn't exist
                os.makedirs(os.path.dirname(file_path) if os.path.dirname(file_path) else '.', exist_ok=True)

                # Open WAV file for writing
                self.wav_file = wave.open(file_path, 'wb')
                self.wav_file.setnchannels(self.channels)
                self.wav_file.setsampwidth(self.bits_per_sample // 8)
                self.wav_file.setframerate(self.sample_rate)

                self.file_path = file_path
                self.is_recording = True
                print(f"Started recording to {file_path}")
                return True

            except Exception as e:
                print(f"Failed to start recording: {e}")
                if self.wav_file:
                    self.wav_file.close()
                    self.wav_file = None
                return False

    def write_audio_frame(self, pcm_data):
        """
        Write PCM audio data to the WAV file.

        Args:
            pcm_data: Bytes object containing PCM audio data

        Returns:
            bool: True if data was written successfully, False otherwise
        """
        with self.lock:
            if not self.is_recording or not self.wav_file:
                return False

            try:
                # Write PCM data to WAV file
                self.wav_file.writeframes(pcm_data)
                return True
            except Exception as e:
                print(f"Error writing audio frame: {e}")
                return False

    def stop_recording(self):
        """
        Stop recording and close the WAV file.

        Returns:
            bool: True if recording stopped successfully, False otherwise
        """
        with self.lock:
            if not self.is_recording or not self.wav_file:
                return False

            try:
                # Close the WAV file
                self.wav_file.close()
                self.wav_file = None
                self.is_recording = False
                print(f"Stopped recording. File saved to {self.file_path}")
                return True
            except Exception as e:
                print(f"Error stopping recording: {e}")
                return False

    def is_active(self):
        """
        Check if recording is currently active.

        Returns:
            bool: True if recording is active, False otherwise
        """
        with self.lock:
            return self.is_recording

    def get_file_path(self):
        """
        Get the current recording file path.

        Returns:
            str: Path to the current recording file, or None if not recording
        """
        with self.lock:
            return self.file_path


class MultiUserAudioWavRecorder:
    """
    A module to record PCM audio data from multiple users to separate WAV files.
    """

    def __init__(self):
        """
        Initialize the multi-user WAV recorder.        
        """
        self.recorders = {}  # Dictionary to store recorders for each user
        self.lock = threading.Lock()

    def get_user_recorder(self, user_id):
        """
        Get or create a recorder for a specific user.

        Args:
            user_id: User identifier

        Returns:
            AudioWavRecorder: Recorder for the specified user
        """
        with self.lock:
            if user_id not in self.recorders:
                self.recorders[user_id] = AudioWavRecorder()
            return self.recorders[user_id]

    def start_recording_for_user(self, user_id, file_path=None, sample_rate=48000, channels=2, bits_per_sample=16):
        """
        Start recording for a specific user.

        Args:
            user_id: User identifier
            file_path: Path to the WAV file. If None, generates a filename with user ID.

        Returns:
            bool: True if recording started successfully, False otherwise
        """
        if file_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            file_path = f"remote_user_{user_id}_{timestamp}.wav"

        recorder = self.get_user_recorder(user_id)
        recorder.sample_rate = sample_rate
        recorder.channels = channels
        recorder.bits_per_sample = bits_per_sample
        return recorder.start_recording(file_path)

    def write_audio_frame_for_user(self, user_id, audio_farme):
        """
        Write PCM audio data for a specific user.

        Args:
            user_id: User identifier
            audio_farme: frame structure of audio data

        Returns:
            bool: True if data was written successfully, False otherwise
        """
        recorder = self.get_user_recorder(user_id)
        # If recorder is not yet started, start it with default filename
        if not recorder.is_active():
            self.start_recording_for_user(user_id,
                sample_rate=audio_farme.samplesPerSec,
                channels=audio_farme.channels,
                bits_per_sample=audio_farme.bytesPerSample * 8)
        return recorder.write_audio_frame(audio_farme.buffer)

    def stop_recording_for_user(self, user_id):
        """
        Stop recording for a specific user.

        Args:
            user_id: User identifier

        Returns:
            bool: True if recording stopped successfully, False otherwise
        """
        with self.lock:
            if user_id in self.recorders:
                self.recorders[user_id].stop_recording()
                del self.recorders[user_id]

    def stop_all_recordings(self):
        """
        Stop recording for all users.
        """
        with self.lock:
            for user_id, recorder in self.recorders.items():
                recorder.stop_recording()

    def is_recording_for_user(self, user_id):
        """
        Check if recording is active for a specific user.

        Args:
            user_id: User identifier

        Returns:
            bool: True if recording is active for the user, False otherwise
        """
        with self.lock:
            if user_id in self.recorders:
                return self.recorders[user_id].is_active()
            return False
