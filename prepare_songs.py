import os #Interacting with operating system (file / directory management)
import subprocess #Runs external programs like demucs
import shutil #Used to copy / move files
import sys #Access to command line arguements?
import json #To store the metadata
import numpy as np 
from madmom.audio.signal import Signal
from madmom.features.beats import RNNBeatProcessor, DBNBeatTrackingProcessor


def convert_wav_to_mp3(wav_path): #Convert wav to mp3 for storage purposes
    mp3_path = wav_path.rsplit(".", 1)[0] + ".mp3"
    
    # Run the conversion
    subprocess.run([
        "ffmpeg", "-y", "-i", wav_path, "-codec:a", "libmp3lame", "-b:a", "320k", mp3_path
    ], check=True)
    
    # Delete the original .wav
    os.remove(wav_path)
    
    return mp3_path


# Path to your folder containing audio files
input_folder = "C:/Personal Computing Stuff/Python/Song splitter/DJ SONGS TO SPLIT"
output_folder = "C:/Personal Computing Stuff/Python/Song splitter/DJ SONGS + STEMS"

# Create the output directory if it doesn't exist
if not os.path.exists(output_folder):
    os.makedirs(output_folder)

# Loop through each file in the input folder
for filename in os.listdir(input_folder):
    if filename.endswith(".mp3"):  # I will be only using mp3's
        input_file = os.path.join(input_folder, filename)
        song_name = os.path.splitext(filename)[0]  #Gets rid of the .mp3 at the end
        song_output_folder = os.path.join(output_folder, song_name) # Makes an output folder that is the name of the song 

        if not os.path.exists(song_output_folder): #Actually constructs the output folder
            os.makedirs(song_output_folder) 

        # Remove any leftover htdemucs directory if it exists  -- need this as if I stop mid split there is an empty htdemucs folder when I restart
        existing_htdemucs = os.path.join(song_output_folder, "htdemucs")
        if os.path.exists(existing_htdemucs):
            shutil.rmtree(existing_htdemucs)

        # Run Demucs to split the song
        subprocess.run([
            "demucs", "--out", song_output_folder, input_file
        ])

        #This puts the files in song_output_folder/htdemucs/song, so need to move the files up a couple times


        # Move files from demucs' extra folders up one level
        demucs_subfolder = os.path.join(song_output_folder, "htdemucs", song_name) #Path of where the files are stored
        if os.path.exists(demucs_subfolder):
            for stem_file in os.listdir(demucs_subfolder):

                full_stem_path = os.path.join(demucs_subfolder, stem_file) #Current path to stem wav file
                dest_path = os.path.join(song_output_folder, stem_file) #Destination path
                shutil.move(full_stem_path, dest_path) # Moves the path up two folders - cleaner organisation

                if dest_path.endswith(".wav"): # Convert to mp3 for space saving purposes
                    try:
                        convert_wav_to_mp3(dest_path)
                    except Exception as e:
                        print(f"Error converting {dest_path} to mp3: {e}")

            shutil.rmtree(os.path.join(song_output_folder, "htdemucs")) #Removes the htdemucs directory (and all subdirectories and files)

        # Copy the original .mp3 into the song's stem folder, and make note of the path
        shutil.copy2(input_file, song_output_folder)
        full_song_path = os.path.join(song_output_folder, filename)

        # Remove the original audio file from the input folder
        try:
            os.remove(input_file)
            print(f"Processed and moved {filename}, output in {song_output_folder}")
        except Exception as e:
            print(f"Error removing {filename}: {e}")



    #Now extract the beatgrid from the file using madmom. Will extract from the mp3 file for now

    print(f"🔍 Processing beatgrid: {full_song_path}")
    act = RNNBeatProcessor()(Signal(full_song_path))

    #Track beats from activation function
    beat_times = DBNBeatTrackingProcessor(fps=100)(act)

    #Estimate BPM
    if len(beat_times) > 1:
        intervals = np.diff(beat_times)
        bpm = 60.0 / np.median(intervals)
    else:
        bpm = 0

    #Output JSON  file
    beatgrid_path = os.path.join(song_output_folder, "beatgrid.json")
    data = {
        "Name": song_name,
        "bpm": round(bpm, 2),
        "beats": list(map(float, beat_times))
    }

    with open(beatgrid_path, "w") as f: #Opens the json file for writing (Will create it if it doesn't exist - which is the case here)
        json.dump(data, f, indent=2) #Dumps the data to the open file f, with indentation for readibility

    print(f"✔️ BPM: {bpm:.2f}")
    print(f"🕒 First 5 beats: {beat_times[:5]}")
    print(f"📁 Saved: {song_name}")
        