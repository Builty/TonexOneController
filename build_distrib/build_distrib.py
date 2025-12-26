import os
import sys
import shutil
import time
from pathlib import Path
from distutils.dir_util import copy_tree
from zipfile import ZipFile

dirname = Path.cwd()

# set version
version = '2.0.2.2_beta_4'

def delete_files_in_folder(directory):
    for filename in os.listdir(directory):
        file_path = os.path.join(directory, filename)
        try:
            if os.path.isfile(file_path) or os.path.islink(file_path):
                os.unlink(file_path)
            elif os.path.isdir(file_path):
                shutil.rmtree(file_path)
        except Exception as e:
            print('Failed to delete %s. Reason: %s' % (file_path, e))
        
def build_distribution(template, target_folder, include_ota, out_filename, skins_path=None):     
    print('BuildDistrib working in folder: ', target_folder)

    # copy default flash updater
    src = os.path.join(dirname, template)
    dest = os.path.join(dirname, 'temp')
    
    print('Delete files in ' + dest)
    delete_files_in_folder(dest)
    
    os.mkdir(os.path.join(dest, 'bin'))
    
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

    # copy partition table
    print('copy partition table...')
    src = os.path.join(dirname, '..', 'source', target_folder, 'partition_table', 'partition-table.bin')
    dest = os.path.join(dirname, 'temp', 'bin')
    print('copying: ' + src + ' to ' + dest)
    shutil.copy(src, dest)

    if include_ota:
        # copy ota_data_initial
        print('copy ota_data_initial...')
        src = os.path.join(dirname, '..', 'source', target_folder, 'ota_data_initial.bin')
        dest = os.path.join(dirname, 'temp', 'bin')
        print('copying: ' + src + ' to ' + dest)
        shutil.copy(src, dest)

    # copy tonex bin
    print('copy tonex bin...')
    src = os.path.join(dirname, '..', 'source', target_folder, 'TonexController.bin')
    dest = os.path.join(dirname, 'temp', 'bin')
    print('copying: ' + src + ' to ' + dest)
    shutil.copy(src, dest)

    if skins_path is not None:
        # copy skins bin
        print('copy skins bin...')
        src = os.path.join(dirname, '..', 'source', 'skin_images', skins_path, 'skins.bin')
        dest = os.path.join(dirname, 'temp', 'bin')
        print('copying: ' + src + ' to ' + dest)
        shutil.copy(src, dest)

    # create zip file
    print('zip files...')
    directory = os.path.join(dirname, 'temp')
    shutil.make_archive(out_filename, 'zip', directory)    

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

# Build Pirate Midi Polar Max (4.3B)
zip_name = 'TonexController_V' + version + '_PirateMidi_PolarMax'
build_distribution('template_cust_partition_16MB', 'build_pirate43B', True, zip_name, '16bit')

print('All done')