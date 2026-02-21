import os
import sys
import shutil
import time
from pathlib import Path
from distutils.dir_util import copy_tree
from zipfile import ZipFile
import json

dirname = Path.cwd()

# set version
version = '2.0.3.2_test_6'

def generate_manifest(merged_path, merged_filename, chip_family, build_name, use_skins):
    manifest = {
        "name": version,
        "version": "Controller",
        "improv": False,
        "new_install_prompt_erase": True, 
        "builds": [
            {
                "chipFamily": chip_family,
                "parts": [
                    {
                        "path": "bootloader.bin",
                        "offset": 0x0000
                    },
                    {
                        "path": "partition-table.bin",
                        "offset": 0x8000
                    },
                    {
                        "path": "TonexController.bin",
                        "offset": 0x10000
                    },
                    {
                        "path": "ota_data_initial.bin",
                        "offset": 0xd000
                    }
                ]
            }
        ]
    }

    if use_skins:
        manifest["builds"][0]["parts"].append({
        "path": "skins.bin",
        "offset": 0x4F2000
    })

    manifest_path = os.path.join(merged_path, 'manifest.json')
    with open(manifest_path, 'w', encoding='utf-8') as f:
        json.dump(manifest, f, indent=2)

    print('Generated manifest.json for %s (chip: %s)' % (build_name, chip_family))
    
def delete_files_in_folder(directory):
    try:
        for filename in os.listdir(directory):
            file_path = os.path.join(directory, filename)
            try:
                if os.path.isfile(file_path) or os.path.islink(file_path):
                    os.unlink(file_path)
                elif os.path.isdir(file_path):
                    shutil.rmtree(file_path)
            except Exception as e:
                print('Failed to delete %s. Reason: %s' % (file_path, e))
    except:
        pass        
        
def build_distribution(template, target_folder, include_ota, out_filename, skins_path=None, skins_name='skins.bin'):     
    print('BuildDistrib working in folder: ', target_folder)

    # copy default flash updater
    src = os.path.join(dirname, template)
    dest = os.path.join(dirname, 'temp')
    
    print('Delete files in ' + dest)
    delete_files_in_folder(dest)
    
    os.mkdir(os.path.join(dest, 'bin'))   
       
    merged_filename = '%s_merged.bin' % out_filename
    
    # make path for merge files
    merged_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), out_filename)
    
    if os.path.isdir(merged_path):        
        delete_files_in_folder(merged_path)
    else:
        os.mkdir(merged_path)

    # copy new files
    print('copy template files from ' + template)
    #copy_tree(src, dest)
    shutil.copytree(src, dest, dirs_exist_ok=True)
               
    # copy bootloader
    print('copy bootloader...')
    src = os.path.join(dirname, '..', 'source', target_folder, 'bootloader', 'bootloader.bin')
    dest = os.path.join(dirname, 'temp', 'bin')
    print('copying: ' + src + ' to ' + dest)
    shutil.copy(src, dest)
    shutil.copy(src, merged_path)

    # copy partition table
    print('copy partition table...')
    src = os.path.join(dirname, '..', 'source', target_folder, 'partition_table', 'partition-table.bin')
    dest = os.path.join(dirname, 'temp', 'bin')
    print('copying: ' + src + ' to ' + dest)
    shutil.copy(src, dest)
    shutil.copy(src, merged_path)

    if include_ota:
        # copy ota_data_initial
        print('copy ota_data_initial...')
        src = os.path.join(dirname, '..', 'source', target_folder, 'ota_data_initial.bin')
        dest = os.path.join(dirname, 'temp', 'bin')
        print('copying: ' + src + ' to ' + dest)
        shutil.copy(src, dest)
        shutil.copy(src, merged_path)

    # copy tonex bin
    print('copy tonex bin...')
    src = os.path.join(dirname, '..', 'source', target_folder, 'TonexController.bin')
    dest = os.path.join(dirname, 'temp', 'bin')
    print('copying: ' + src + ' to ' + dest)
    shutil.copy(src, dest)
    shutil.copy(src, merged_path)

    if skins_path is not None:
        # copy skins bin
        print('copy skins bin...')
        src = os.path.join(dirname, '..', 'source', 'skin_images', skins_path, skins_name)
        dest = os.path.join(dirname, 'temp', 'bin', 'skins.bin')
        print('copying: ' + src + ' to ' + dest)
        shutil.copy(src, dest)
        shutil.copy(src, merged_path)

    # create zip file
    print('zip files...')
    directory = os.path.join(dirname, 'temp')
    shutil.make_archive(out_filename, 'zip', directory)    
    
    # generate manifest file for web tool
    generate_manifest(merged_path, merged_filename, "ESP32-S3", out_filename, skins_path is not None)
        
    print('Build complete\n\n')
    
# Build Waveshare 1.69" 
zip_name = 'TonexController_V' + version + '_Waveshare_1_69'
build_distribution('template_cust_partition', 'build_ws169', True, zip_name)

# Build Waveshare 1.69" landscape
zip_name = 'TonexController_V' + version + '_Waveshare_1_69land'
build_distribution('template_cust_partition', 'build_ws169land', True, zip_name)

# Build Waveshare 1.69" Touch
zip_name = 'TonexController_V' + version + '_Waveshare_1_69_Touch'
build_distribution('template_cust_partition', 'build_ws169t', True, zip_name)

# Build Waveshare 1.69" Touch landscape
zip_name = 'TonexController_V' + version + '_Waveshare_1_69_Touch_land'
build_distribution('template_cust_partition', 'build_ws169tland', True, zip_name)

# Build Waveshare 4.3B
zip_name = 'TonexController_V' + version + '_Waveshare_4_3B'
build_distribution('template_cust_partition_16MB', 'build_ws43b', True, zip_name, '16bit')

# Build Waveshare 7/4.3 non-B
zip_name = 'TonexController_V' + version + '_Waveshare_7_43'
build_distribution('template_cust_partition_8MB', 'build_ws7_43', True, zip_name, '16bit', 'skins_minimal.bin')

# Build Waveshare Zero
zip_name = 'TonexController_V' + version + '_Waveshare_Zero'
build_distribution('template_cust_partition', 'build_wszero', True, zip_name)

# Build Devkit C N8R2
zip_name = 'TonexController_V' + version + '_DevKitC_N8R2'
build_distribution('template_cust_partition', 'build_devkitc_N8R2', True, zip_name)

# Build Devkit C N16R8
zip_name = 'TonexController_V' + version + '_DevKitC_N16R8'
build_distribution('template_cust_partition', 'build_devkitc_N16R8', True, zip_name)

# Build M5 Atom
zip_name = 'TonexController_V' + version + '_M5AtomS3R'
build_distribution('template_cust_partition', 'build_m5atoms3r', True, zip_name)

# Build Lilygo T-Display S3
zip_name = 'TonexController_V' + version + '_Lilygo_TDisplay_S3'
build_distribution('template_cust_partition', 'build_lgtdisps3', True, zip_name)

# Build Waveshare 3.5B
zip_name = 'TonexController_V' + version + '_Waveshare_3_5B'
build_distribution('template_cust_partition_16MB', 'build_ws35b', True, zip_name, '16bitswapped')

# Build JC3248W
zip_name = 'TonexController_V' + version + '_JC3248W'
build_distribution('template_cust_partition_16MB', 'build_jc3248w', True, zip_name, '16bitswapped')

# Build Waveshare 1.9
zip_name = 'TonexController_V' + version + '_Waveshare_1_9'
build_distribution('template_cust_partition', 'build_ws19t', True, zip_name)

# Build Pirate Midi Polar Pico (Zero)
zip_name = 'TonexController_V' + version + '_PirateMidi_PolarPico'
build_distribution('template_cust_partition', 'build_piratezero', True, zip_name)

# Build Pirate Midi Polar Mini (1.69")
zip_name = 'TonexController_V' + version + '_PirateMidi_PolarMini'
build_distribution('template_cust_partition', 'build_pirate169', True, zip_name)

# Build Pirate Midi Polar Plus (1.69" landscape)
zip_name = 'TonexController_V' + version + '_PirateMidi_PolarPlus'
build_distribution('template_cust_partition', 'build_pirate169land', True, zip_name)

# Build Pirate Midi Polar Mini V2 (1.69")
zip_name = 'TonexController_V' + version + '_PirateMidi_PolarMiniV2'
build_distribution('template_cust_partition', 'build_pirateminiv2', True, zip_name)

# Build Pirate Midi Polar Plus V2 (1.69")
zip_name = 'TonexController_V' + version + '_PirateMidi_PolarPlusV2'
build_distribution('template_cust_partition', 'build_pirateplusv2', True, zip_name)

# Build Pirate Midi Polar Max (4.3B)
zip_name = 'TonexController_V' + version + '_PirateMidi_PolarMax'
build_distribution('template_cust_partition_16MB', 'build_pirate43B', True, zip_name, '16bit')

# Build Pirate Midi Polar Pro (1.69")
zip_name = 'TonexController_V' + version + '_PirateMidi_PolarPro'
build_distribution('template_cust_partition', 'build_piratepro', True, zip_name)

print('All done')