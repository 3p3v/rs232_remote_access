param(
    [String]$port
)

# REGION MAIN
    $src_folder = ".conf\"
    $image_size = 0x10000
    $offset = 0x200000

    if (!$port) {
        Write-Host("Did not set port, returning!")
        return
    }

    Write-Host("Creating image...")
    ./mkspiffs.exe -c $src_folder -b 4096 -p 256 -s $image_size ./build/certs.bin

    Write-Host("Image created, uploading to device...")
    ./esptool.exe --chip esp32 -p $port -b 460800 write_flash -z $offset ./build/certs.bin
# REGION MAIN END