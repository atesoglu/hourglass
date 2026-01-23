# Sound Files

The application uses embedded sound files for timer completion notifications. Place the following WAV files in the `sounds/` directory:

- `chime.wav` - Pleasant chime sound (default)
- `beep.wav` - Simple beep tone
- `bell.wav` - Bell/chime variation

These files are embedded as Qt resources via `resources.qrc`. If the sound files are missing, the application will fall back to the system beep.

## Adding Sound Files

1. Place WAV files in the `sounds/` directory
2. Ensure they are listed in `resources.qrc`
3. Rebuild the application

The application will work without these files (using system beep as fallback), but for the best experience, add the sound files.
