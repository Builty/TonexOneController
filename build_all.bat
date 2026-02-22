# build_all.ps1
# Automates clean + build for all ESP-IDF variants defined in your project
#Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned

# ==================== CONFIGURATION ====================

$IdfExportPath = "F:\projects\Expressif\svn\v5.5.1\esp-idf\export.ps1"

# If the path above is wrong → script will stop with clear error
if (-not (Test-Path $IdfExportPath)) {
    Write-Host "ERROR: Cannot find ESP-IDF export script at: $IdfExportPath" -ForegroundColor Red
    Write-Host "Please update `$IdfExportPath` in this script to point to your export.ps1" -ForegroundColor Yellow
    pause
    exit 1
}

# Project root (where CMakeLists.txt lives)
$ProjectDir = Get-Location

# Variants: @(
#   @{ Name = "..."; Defaults = "..."; BuildDir = "..."; SdkConfigPath = "..." }
# )
$variants = @(
    @{ Name = "WS169";           Defaults = "sdkconfig.defaults;sdkconfig.ws169";        BuildDir = "build_ws169";        SdkConfigPath = "build_ws169" }
    @{ Name = "WS169Land";       Defaults = "sdkconfig.defaults;sdkconfig.ws169land";    BuildDir = "build_ws169land";    SdkConfigPath = "build_ws169land" }
    @{ Name = "WS169TOUCH";      Defaults = "sdkconfig.defaults;sdkconfig.ws169t";       BuildDir = "build_ws169t";       SdkConfigPath = "build_ws169t" }
    @{ Name = "WS169TOUCHLand";  Defaults = "sdkconfig.defaults;sdkconfig.ws169tland";   BuildDir = "build_ws169tland";   SdkConfigPath = "build_ws169tland" }
    @{ Name = "WS43B";           Defaults = "sdkconfig.defaults;sdkconfig.ws43B";       BuildDir = "build_ws43b";        SdkConfigPath = "build_ws43b" }
    @{ Name = "WS35B";           Defaults = "sdkconfig.defaults;sdkconfig.ws35b";       BuildDir = "build_ws35b";        SdkConfigPath = "build_ws35b" }
    @{ Name = "JC3248W";         Defaults = "sdkconfig.defaults;sdkconfig.jc3248w";     BuildDir = "build_jc3248w";      SdkConfigPath = "build_jc3248w" }
    @{ Name = "WSZERO";          Defaults = "sdkconfig.defaults;sdkconfig.wszero";      BuildDir = "build_wszero";       SdkConfigPath = "build_wszero" }
    @{ Name = "DEVKITC_N8R2";    Defaults = "sdkconfig.defaults;sdkconfig.devkitc_N8R2"; BuildDir = "build_devkitc_N8R2";   SdkConfigPath = "build_devkitc_N8R2" }
    @{ Name = "DEVKITC_N16R8";   Defaults = "sdkconfig.defaults;sdkconfig.devkitc_N16R8";BuildDir = "build_devkitc_N16R8";  SdkConfigPath = "build_devkitc_N16R8" }
    @{ Name = "M5AtomS3R";       Defaults = "sdkconfig.defaults;sdkconfig.m5atoms3r";   BuildDir = "build_m5atoms3r";    SdkConfigPath = "build_m5atoms3r" }
    @{ Name = "LGTDisplayS3";    Defaults = "sdkconfig.defaults;sdkconfig.lgtdisps3";   BuildDir = "build_lgtdisps3";    SdkConfigPath = "build_lgtdisps3" }
    @{ Name = "WS19Touch";       Defaults = "sdkconfig.defaults;sdkconfig.ws19t";       BuildDir = "build_ws19t";        SdkConfigPath = "build_ws19t" }
    @{ Name = "WS7_43";          Defaults = "sdkconfig.defaults;sdkconfig.ws43devonly"; BuildDir = "build_ws7_43";       SdkConfigPath = "build_ws7_43" }
    @{ Name = "PirateMini";      Defaults = "sdkconfig.defaults;sdkconfig.pirate169";   BuildDir = "build_pirate169";    SdkConfigPath = "build_pirate169" }
    @{ Name = "PiratePlus";      Defaults = "sdkconfig.defaults;sdkconfig.pirate169land";BuildDir = "build_pirate169land"; SdkConfigPath = "build_pirate169land" }
    @{ Name = "PirateZERO";      Defaults = "sdkconfig.defaults;sdkconfig.piratezero";  BuildDir = "build_piratezero";   SdkConfigPath = "build_piratezero" }
    @{ Name = "Pirate43B";       Defaults = "sdkconfig.defaults;sdkconfig.pirate43B";   BuildDir = "build_pirate43B";    SdkConfigPath = "build_pirate43B" }
    @{ Name = "PirateMiniV2";    Defaults = "sdkconfig.defaults;sdkconfig.pirateminiv2";BuildDir = "build_pirateminiv2";  SdkConfigPath = "build_pirateminiv2" }
    @{ Name = "PiratePlusV2";    Defaults = "sdkconfig.defaults;sdkconfig.pirateplusv2";BuildDir = "build_pirateplusv2";  SdkConfigPath = "build_pirateplusv2" }
    @{ Name = "PiratePro";       Defaults = "sdkconfig.defaults;sdkconfig.piratepro";   BuildDir = "build_piratepro";    SdkConfigPath = "build_piratepro" }
    @{ Name = "PirateMaxV2";     Defaults = "sdkconfig.defaults;sdkconfig.piratemax35"; BuildDir = "build_piratemax35";   SdkConfigPath = "build_piratemax35" }
)

# ==================== MAIN LOGIC ====================

Write-Host "Activating ESP-IDF environment..." -ForegroundColor Cyan
. $IdfExportPath

Write-Host "ESP-IDF environment activated" -ForegroundColor Green
Write-Host ""

foreach ($variant in $variants) {
    $name          = $variant.Name
    $defaults      = $variant.Defaults
    $buildDir      = $variant.BuildDir
    $sdkconfigPath = Join-Path $ProjectDir $variant.SdkConfigPath "sdkconfig"

    Write-Host ("=" * 60) -ForegroundColor Magenta
    Write-Host "Variant: $name" -ForegroundColor Cyan
    Write-Host ("=" * 60) -ForegroundColor Magenta

    $fullBuildPath = Join-Path $ProjectDir $buildDir

    # Create build directory if it doesn't exist
    if (-not (Test-Path $fullBuildPath)) {
        New-Item -ItemType Directory -Path $fullBuildPath | Out-Null
    }

    # Set environment variables for this variant
    $env:SDKCONFIG_DEFAULTS = $defaults
    $env:SDKCONFIG          = $sdkconfigPath

    Write-Host "  SDKCONFIG_DEFAULTS = $env:SDKCONFIG_DEFAULTS"
    Write-Host "  SDKCONFIG          = $env:SDKCONFIG"
    Write-Host ""

    # Full clean
    Write-Host "Cleaning..." -ForegroundColor Yellow
    idf.py -B "$fullBuildPath" fullclean

    # Build (reconfigures automatically if needed)
    Write-Host "Building..." -ForegroundColor Yellow
    idf.py -B "$fullBuildPath" build

    if ($LASTEXITCODE -ne 0) {
        Write-Host "Build failed for $name !" -ForegroundColor Red
        Write-Host "Stopping here." -ForegroundColor Red
        pause
        exit 1
    }

    Write-Host "Variant $name completed successfully" -ForegroundColor Green
    Write-Host ""
}

Write-Host "ALL VARIANTS BUILT SUCCESSFULLY!" -ForegroundColor Green
Write-Host ""
pause