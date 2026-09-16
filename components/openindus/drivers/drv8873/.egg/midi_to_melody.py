#!/usr/bin/env python3
"""
MIDI to Melody Header Converter

This script converts MIDI files to C header files containing melody data
suitable for microcontroller applications.

Usage:
    python midi_to_melody.py input.mid [output.h]

Dependencies:
    pip install mido
"""

import mido
import sys
import os
import argparse
from pathlib import Path

def midi_note_to_frequency(note_num):
    """Convert MIDI note number to frequency in Hz"""
    if note_num == 0:  # Rest
        return 0
    return 440.0 * (2.0 ** ((note_num - 69) / 12.0))

def get_note_name(note_num):
    """Get note name from MIDI note number"""
    if note_num == 0:
        return "REST"
    
    note_names = {
        # Octave 2 (typical range)
        36: 'C2', 37: 'C#2', 38: 'D2', 39: 'D#2', 40: 'E2', 41: 'F2',
        42: 'F#2', 43: 'G2', 44: 'G#2', 45: 'A2', 46: 'A#2', 47: 'B2',
        # Octave 3
        48: 'C3', 49: 'C#3', 50: 'D3', 51: 'D#3', 52: 'E3', 53: 'F3',
        54: 'F#3', 55: 'G3', 56: 'G#3', 57: 'A3', 58: 'A#3', 59: 'B3',
        # Octave 4
        60: 'C4', 61: 'C#4', 62: 'D4', 63: 'D#4', 64: 'E4', 65: 'F4',
        66: 'F#4', 67: 'G4', 68: 'G#4', 69: 'A4', 70: 'A#4', 71: 'B4',
        # Octave 5
        72: 'C5', 73: 'C#5', 74: 'D5', 75: 'D#5', 76: 'E5', 77: 'F5',
        78: 'F#5', 79: 'G5', 80: 'G#5', 81: 'A5', 82: 'A#5', 83: 'B5',
        # Octave 6
        84: 'C6', 85: 'C#6', 86: 'D6', 87: 'D#6', 88: 'E6', 89: 'F6',
        90: 'F#6', 91: 'G6', 92: 'G#6', 93: 'A6', 94: 'A#6', 95: 'B6',
        # Octave 7
        96: 'C7', 97: 'C#7', 98: 'D7', 99: 'D#7', 100: 'E7', 101: 'F7',
        102: 'F#7', 103: 'G7', 104: 'G#7', 105: 'A7', 106: 'A#7', 107: 'B7',
    }
    
    return note_names.get(note_num, f"MIDI_{note_num}")

def get_tempo(mid_file):
    """Get tempo from MIDI file (microseconds per beat)"""
    tempo_us = 500000  # Default 120 BPM
    for track in mid_file.tracks:
        for msg in track:
            if msg.type == 'set_tempo':
                tempo_us = msg.tempo  # Return microseconds per beat directly
                break
        if tempo_us != 500000:
            break
    return tempo_us

def process_midi_file(midi_file_path):
    """Process MIDI file and extract note sequence with rest support"""
    mid = mido.MidiFile(midi_file_path)
    
    # Use the first track or merge all tracks
    if len(mid.tracks) == 0:
        raise ValueError("No tracks found in MIDI file")
    
    # Get tempo information
    tempo_us = get_tempo(mid)
    
    # Process each track separately to maintain proper timing
    all_notes = []
    
    for track in mid.tracks:
        # Track note on/off events within this track
        active_notes = {}  # note_num -> start_time_ticks
        melody_notes = []
        current_time = 0
        
        for msg in track:
            # Update current time with delta time for ALL messages
            current_time += msg.time
            
            if msg.type == 'note_on' and msg.velocity > 0:
                # Note start - store the time when note was triggered
                active_notes[msg.note] = current_time
            elif (msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0)) and msg.note in active_notes:
                # Note end
                start_time = active_notes[msg.note]
                duration_ticks = current_time - start_time
                
                # Convert ticks to milliseconds using tempo
                if duration_ticks == 0:
                    duration_ms = 100  # Minimum duration for zero-length notes
                else:
                    # Correct duration calculation using microseconds:
                    # milliseconds = (duration_ticks / ticks_per_beat) * (tempo_us / 1000)
                    beats = duration_ticks / mid.ticks_per_beat
                    duration_ms = max(1, int(beats * (tempo_us / 1000.0)))
                
                # Check if there are other active notes playing at the same time
                # If so, prioritize the higher note (melody) over lower notes (bass)
                other_active_notes = [note for note in active_notes.keys() if note != msg.note]
                
                if not other_active_notes:
                    # No chord - add this note normally with both start and end times
                    melody_notes.append({
                        'note': msg.note,
                        'frequency': int(midi_note_to_frequency(msg.note)),
                        'duration_ms': duration_ms,
                        'start_time': start_time,
                        'end_time': current_time
                    })
                
                del active_notes[msg.note]
        
        # Sort notes by start time to ensure correct ordering
        melody_notes.sort(key=lambda x: x['start_time'])
        
        # Now add rests for gaps between notes
        final_notes = []
        for i in range(len(melody_notes)):
            # Check if we should add a rest BEFORE this note (gap from previous note)
            if i > 0:
                prev_note_end = melody_notes[i - 1]['end_time']
                current_note_start = melody_notes[i]['start_time']
                
                # Calculate the gap between notes
                gap_ticks = current_note_start - prev_note_end
                if gap_ticks > 0:
                    gap_beats = gap_ticks / mid.ticks_per_beat
                    gap_duration_ms = int(gap_beats * (tempo_us / 1000.0))
                    
                    # Only add significant rests (>50ms to filter noise)
                    if gap_duration_ms > 50:
                        final_notes.append({
                            'note': 0,
                            'frequency': 0,  # Rest = frequency 0
                            'duration_ms': gap_duration_ms
                        })
            
            # Add the note itself
            final_notes.append({
                'note': melody_notes[i]['note'],
                'frequency': melody_notes[i]['frequency'],
                'duration_ms': melody_notes[i]['duration_ms']
            })
        
        # Add notes from this track to the overall list
        all_notes.extend(final_notes)
    
    return all_notes

def generate_header_file(melody_notes, output_path, melody_name="melody"):
    """Generate C header file from melody data"""
    
    header_content = f"""/*
 * Auto-generated melody header file
 * Generated from MIDI file
 * 
 * Usage:
 *   struct Note {{
 *     int freq;
 *     int durationMs;
 *   }};
 *   
 *   const Note {melody_name}[] = {{
 *     // Melody data
 *   }};
 *   
 *   int {melody_name}_length = sizeof({melody_name}) / sizeof({melody_name}[0]);
 */
 
#ifndef {melody_name.upper()}_H
#define {melody_name.upper()}_H

#ifdef __cplusplus
extern "C" {{
#endif

struct Note {{
    int freq;
    int durationMs;
}};

"""
    
    header_content += f"const Note {melody_name}[] = {{\n"
    
    for note_data in melody_notes:
        freq = note_data['frequency']
        duration = note_data['duration_ms']
        midi_note = note_data['note']
        
        # Use the new note naming function
        note_name = get_note_name(midi_note)
        header_content += f"    {{ {freq:>3}, {duration:>3} }}, // {note_name}\n"
    
    header_content += f"}};\n\n"
    header_content += f"int {melody_name}_length = sizeof({melody_name}) / sizeof({melody_name}[0]);\n\n"
    header_content += "#ifdef __cplusplus\n}\n#endif\n\n#endif\n"
    
    with open(output_path, 'w') as f:
        f.write(header_content)
    
    return len(melody_notes)

def main():
    parser = argparse.ArgumentParser(description='Convert MIDI files to melody header files')
    parser.add_argument('input', help='Input MIDI file path')
    parser.add_argument('output', nargs='?', help='Output header file path (default: input_name.h)')
    parser.add_argument('--name', default='melody', help='Melody variable name (default: melody)')
    
    args = parser.parse_args()
    
    # Check if input file exists
    if not os.path.exists(args.input):
        print(f"Error: Input file '{args.input}' not found")
        sys.exit(1)
    
    # Set default output path
    if args.output is None:
        input_path = Path(args.input)
        args.output = str(input_path.with_suffix('.h'))
    
    try:
        print(f"Processing MIDI file: {args.input}")
        
        # Process MIDI file
        melody_notes = process_midi_file(args.input)
        
        if not melody_notes:
            print("Warning: No notes found in MIDI file")
            sys.exit(1)
        
        print(f"Extracted {len(melody_notes)} notes")
        
        # Generate header file
        note_count = generate_header_file(melody_notes, args.output, args.name)
        
        print(f"Generated header file: {args.output}")
        print(f"Melody variable name: {args.name}")
        print(f"Note count: {note_count}")
        
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()
