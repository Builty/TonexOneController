import os
import sys
import shutil
import time
from pathlib import Path
from distutils.dir_util import copy_tree
from zipfile import ZipFile

dirname = Path.cwd()

# set version
version = '1.0.4.2'

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
        
def build_distribution(template, target_folder, include_ota, out_filename):     
    print('BuildDistrib working in folder: ', target_folder)

    # copy default flash updater
    src = os.path.join(dirname, template)
    dest = os.path.join(dirname, 'temp')
    
    print('Delete files in ' + dest)
    delete_files_in_folder(dest)
           
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


    # create zip file
    print('zip files...')
    directory = os.path.join(dirname, 'temp')
    shutil.make_archive(out_filename, 'zip', directory)    

    print('Build complete\n\n')
    
# Build Waveshare 1.69" 
zip_name = 'TonexOneController_V' + version + '_Waveshare_1_69'
build_distribution('template_cust_partition', 'build_ws169', True, zip_name)

# Build Waveshare 4.3B with Amp skins
zip_name = 'TonexOneController_V' + version + '_Waveshare_4_3B_Display_Amp_Skins'
build_distribution('template_cust_partition', 'build_ws43b_as', True, zip_name)

# Build Waveshare 4.3B with Pedal skins
zip_name = 'TonexOneController_V' + version + '_Waveshare_4_3B_Display_Pedal_Skins'
build_distribution('template_cust_partition', 'build_ws43b_ps', True, zip_name)

# Build Waveshare Zero
zip_name = 'TonexOneController_V' + version + '_Waveshare_Zero'
build_distribution('template_def_partition', 'build_wszero', False, zip_name)

print('All done')