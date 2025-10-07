import os
import struct
import argparse

# List of input binary files
# Must match indexes/order with controller code!    
files = [
        "jcm.bin",          # AMP_SKIN_JCM,
        "slvrface.bin",     # AMP_SKIN_SILVERFACE,
        "tnxablk.bin",      # AMP_SKIN_TONEXAMPBLACK,
        "5150.bin",         # AMP_SKIN_5150,
        "ampgchrm.bin",     # AMP_SKIN_AMPEGCHROME,
        "fndrtwdg.bin",     # AMP_SKIN_FENDERTWEEDBIG,
        "fndrhtrd.bin",     # AMP_SKIN_FENDERHOTROD,
        "msbogdul.bin",     # AMP_SKIN_MESABOOGIEDUAL,
        "elgntblu.bin",     # AMP_SKIN_ELEGANTBLUE,
        "mdnwhplx.bin",     # AMP_SKIN_MODERNWHITEPLEXI,
        "roljazz.bin",      # AMP_SKIN_ROLANDJAZZ,
        "orngr120.bin",     # AMP_SKIN_ORANGEOR120,
        "mdnbkplx.bin",     # AMP_SKIN_MODERNBLACKPLEXI,
        "fndrtwin.bin",     # AMP_SKIN_FENDERTWIN,
        "ba500.bin",        # AMP_SKIN_BA500,
        "msamkwd.bin",      # AMP_SKIN_MESAMARKWOOD,
        "mesamkv.bin",      # AMP_SKIN_MESAMARKV,
        "jtm.bin",          # AMP_SKIN_JTM,
        "jbdumble.bin",     # AMP_SKIN_JBDUMBLE1,
        "jetcity.bin",      # AMP_SKIN_JETCITY,
        "ac30.bin",         # AMP_SKIN_AC30,
        "evh.bin",          # AMP_SKIN_EVH,
        "tnxared.bin",      # AMP_SKIN_TONEXAMPRED,
        "friedman.bin",     # AMP_SKIN_FRIEDMANN,
        "supro.bin",        # AMP_SKIN_SUPRO,
        "diezel.bin",       # AMP_SKIN_DIEZEL,
        "whtmdrn.bin",      # AMP_SKIN_WHITEMODERN,
        "woodamp.bin",      # AMP_SKIN_WOODAMP,
        "bigmuff.bin",      # PEDAL_SKIN_BIGMUFF,
        "bossblk.bin",      # PEDAL_SKIN_BOSSBLACK,
        "bossslvr.bin",     # PEDAL_SKIN_BOSSSILVER,
        "bossyel.bin",      # PEDAL_SKIN_BOSSYELLOW,
        "fuzzred.bin",      # PEDAL_SKIN_FUZZRED,
        "fuzzslvr.bin",     # PEDAL_SKIN_FUZZSILVER,
        "ibnzblue.bin",     # PEDAL_SKIN_IBANEZBLUE,
        "ibnzdblu.bin",     # PEDAL_SKIN_IBANEZDARKBLUE,
        "ibnzgrn.bin",      # PEDAL_SKIN_IBANEZGREEN,
        "ibnzred.bin",      # PEDAL_SKIN_IBANEZRED,
        "klongld.bin",      # PEDAL_SKIN_KLONGOLD,
        "lifepdl.bin",      # PEDAL_SKIN_LIFEPEDAL,
        "mngglry.bin",      # PEDAL_SKIN_MORNINGGLORY,
        "mxrdbbl.bin",      # PEDAL_SKIN_MXRDOUBLEBLACK,
        "mxrdblrd.bin",     # PEDAL_SKIN_MXRDOUBLERED,
        "mxrsngbk.bin",     # PEDAL_SKIN_MXRSINGLEBLACK,
        "mxrsnggd.bin",     # PEDAL_SKIN_MXRSINGLEGOLD,
        "mxrsnggn.bin",     # PEDAL_SKIN_MXRSINGLEGREEN,
        "mxrsgorg.bin",     # PEDAL_SKIN_MXRSINGLEORANGE,
        "mxrsngwh.bin",     # PEDAL_SKIN_MXRSINGLEWHITE,
        "mxrsngyl.bin",     # PEDAL_SKIN_MXRSINGLEYELLOW,
        "ratyell.bin"       # PEDAL_SKIN_RATYELLOW,
    ]
output_bin = "skins.bin"

# TOC entry format: offset (4 bytes), size (4 bytes)
TOC_ENTRY_SIZE = 4 + 4  # 4 for offset, 4 for size
DEFAULT_MAX_FILES = 100  # Maximum number of files in TOC
TOC_SIZE = TOC_ENTRY_SIZE * DEFAULT_MAX_FILES

def main():
     # Parse command-line arguments
    parser = argparse.ArgumentParser(description="Concatenate Skin image files with embedded TOC")
    parser.add_argument('--max-files', type=int, default=DEFAULT_MAX_FILES, help="Maximum number of files in TOC (default: 100)")
    args = parser.parse_args()
    max_files = args.max_files
    
    print("Skins max files: %d" % max_files)
    
    # Collect file info
    toc = []
    current_offset = TOC_SIZE  # Start data after TOC
    for f in files:
        if not os.path.exists(f):
            raise FileNotFoundError("File not found %s", f)
        size = os.path.getsize(f)
        toc.append((current_offset, size))
        current_offset += size

    # Write output binary
    with open(output_bin, "wb") as out:
        # Write TOC
        for i in range(DEFAULT_MAX_FILES):
            if i < max_files:
                offset, size = toc[i]
                               
                # Pack entry: name (16s), offset (I), size (I)
                out.write(struct.pack('<II', offset, size))
            else:
                # Pad with empty entries
                out.write(struct.pack('<II', 0, 0))

        # Write file data
        file_count = 0
        for f in files:
            if file_count == max_files:
                break
                
            with open(f, "rb") as infile:
                out.write(infile.read())
                
            file_count += 1    

    print("Skins Generated with embedded TOC")
    print("TOC entries:")
    for offset, size in toc:
        print("  offset=%d, size=%d" % (offset, size))

if __name__ == "__main__":
    main()