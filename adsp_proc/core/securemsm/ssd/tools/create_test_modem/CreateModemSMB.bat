:: Copyright (c) 2010 by QUALCOMM Incorporated. All Rights Reserved 

@echo off
set local

set ENCRYPT_ERROR=0

pushd .

set TOOLS_DIR=..
set RSA_KEYS_DIR=%TOOLS_DIR%\gen_rsa_keys
set KEYDIR_DEV=%RSA_KEYS_DIR%\Key_Pair_golden_dev
set KEYDIR_OEM=%RSA_KEYS_DIR%\Key_Pair_golden_oem
set GEN_RAND_DIR=%TOOLS_DIR%\gen_rand
set RAND_SEED=rand_seed.dat
set RAND_SEED_IV=rand_seed_iv.dat
set GEN_METADATA_DIR=%TOOLS_DIR%\gen_metadata
set CONFIG_FILE=md_config.ini

set INPUT_IMAGE_DIR=%TOOLS_DIR%\create_test_modem
set INPUT_IMAGE=inputImage.smb
set TEMP_ENC_IMAGE=inputImage.enc
set ENCRYPTED_IMAGE=fotaui.mbn
set MAGIC_NUMBER=magic.mbn

if NOT EXIST "%INPUT_IMAGE%" (
	@echo ERROR: fotapacker failed to create "%INPUT_IMAGE%"
	set ENCRYPT_ERROR=1
	goto encrypt_cleanup
)

:start_encrypt
set KEY=""
set IV=""

:: Before we start encryption make sure the required files are present
if NOT EXIST %KEYDIR_OEM%/rsa_pr_key_golden_oem.pem (
	@echo ERROR: %KEYDIR_OEM%/rsa_pr_key_oem.pem not found
	set ENCRYPT_ERROR=1
	goto encrypt_cleanup
)

if NOT EXIST %KEYDIR_DEV%/rsa_pub_key_golden_dev.der (
	@echo ERROR: rsa_pub_key_dev.der not found
	set ENCRYPT_ERROR=1
	goto encrypt_cleanup
)
if NOT EXIST %KEYDIR_DEV%/rsa_pub_key_golden_dev.pem (
	@echo ERROR: rsa_pub_key_dev.pem not found
	set ENCRYPT_ERROR=1
	goto encrypt_cleanup
)

%GEN_RAND_DIR%\gen_rand.exe > NUL 2>&1
if %ERRORLEVEL% EQU 9009 (
	@echo ERROR: gen_rand.exe not found in %GEN_RAND_DIR%
	set ENCRYPT_ERROR=1
	goto encrypt_cleanup
)

copy %GEN_RAND_DIR%\gen_rand.exe /Y

gen_rand.exe 1024 > %RAND_SEED%

if NOT EXIST %RAND_SEED% (
	@echo ERROR: %RAND_SEED% not found
	set ENCRYPT_ERROR=1
	goto encrypt_cleanup
)

gen_rand.exe 1024 > %RAND_SEED_IV%

if NOT EXIST %RAND_SEED_IV% (
	@echo ERROR: %RAND_SEED_IV% not found
	set ENCRYPT_ERROR=1
	goto encrypt_cleanup
)

:: Not needed for testing.
if NOT EXIST %GEN_METADATA_DIR%\%CONFIG_FILE% (
	@echo ERROR: %GEN_METADATA_DIR%\%CONFIG_FILE% not found
	set ENCRYPT_ERROR=1
	goto encrypt_cleanup
)

copy %GEN_METADATA_DIR%\%CONFIG_FILE% /Y

:: Make sure all the required tools exist
openssl version
if %ERRORLEVEL% EQU 9009 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
  @echo ERROR: %ERRORLEVEL% openssl must be installed and in path
	goto encrypt_cleanup
)


%GEN_METADATA_DIR%\gen_metadata.exe > NUL 2>&1
if %ERRORLEVEL% EQU 9009 (
	@echo ERROR: gen_metadata.exe not found in %GEN_METADATA_DIR%
	set ENCRYPT_ERROR=1
	goto encrypt_cleanup
)

copy %GEN_METADATA_DIR%\gen_metadata.exe /Y


Bin2Hex > NUL 2>&1
if %ERRORLEVEL% EQU 9009 (
	@echo ERROR: Bin2Hex.exe not found in %TOOLS_DIR%\create_test_modem
	set ENCRYPT_ERROR=1
	goto encrypt_cleanup
)

:: Generate aes-128 key and initial value for encryption
openssl rand 16 -out aes128.key -rand %RAND_SEED%
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
	@echo ERROR: openssl rand failed to create aes128 key
	goto encrypt_cleanup
)

openssl rand 16 -out aes128.iv -rand %RAND_SEED_IV%
if NOT EXIST aes128.iv (
@echo ERROR: openssl rand failed to create aes128 iv
set ENCRYPT_ERROR=1
	goto encrypt_cleanup
)

bin2hex aes128.key > aes128_key.dat
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
	@echo ERROR: %ERRORLEVEL% bin2hex failed to translate key
	goto encrypt_cleanup
)

bin2hex aes128.iv > aes128_iv.dat
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
	@echo ERROR: bin2hex failed to translate iv
	goto encrypt_cleanup
)

for /f "tokens=1 delims=" %%a in (aes128_key.dat) do set KEY=%%a
for /f "tokens=1 delims=" %%a in (aes128_iv.dat) do set IV=%%a

if %KEY% EQU "" (
	@echo ERROR: Failed to set KEY
	set ENCRYPT_ERROR=1
	goto encrypt_cleanup
)

if %IV% EQU "" (
	@echo ERROR: Failed to set IV
	set ENCRYPT_ERROR=1
	goto encrypt_cleanup
)

:: Encode Device Public Key Id in base64
openssl enc -base64 -A -in %KEYDIR_DEV%/rsa_pub_key_id_golden_dev.dat -out pub_key_id_dev.dat
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
  @echo ERROR: openssl enc -base64 reported an error
	goto encrypt_cleanup
)

:: Encode OEM Public Key Id in base64
openssl enc -base64 -A -in %KEYDIR_OEM%/rsa_pub_key_id_golden_oem.dat -out pub_key_id_oem.dat
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
  @echo ERROR: openssl enc -base64 reported an error
	goto encrypt_cleanup
)

:: encrypt the full input image using aes-128-cbc
openssl enc -aes-128-cbc -in %INPUT_IMAGE% -K %KEY% -iv %IV% -out %TEMP_ENC_IMAGE%
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
	@echo ERROR: openssl enc reported an error
	goto encrypt_cleanup
)

:: Now encrypt the aes-128 key that was used to encrypt the modem with the device public key
openssl rsautl -encrypt -pkcs -in aes128.key -inkey %KEYDIR_DEV%/rsa_pub_key_golden_dev.pem -pubin -out temp.dat
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
	@echo ERROR: openssl rsautl -encrypt reported an error
	goto encrypt_cleanup
)

openssl enc -base64 -A -in temp.dat -out iek_key.enc
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
	@echo ERROR: openssl enc reported an error
	goto encrypt_cleanup
)

:: Generate the hash of the encoded INPUT_IMAGE
 openssl dgst -sha256 -binary %TEMP_ENC_IMAGE% | openssl enc -base64 -A -out img_hash.dat
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
  @echo ERROR: openssl dgst reported an error
	goto encrypt_cleanup
)

openssl enc -base64 -A -in aes128.iv -out aes128_iv.dat
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
	@echo ERROR: openssl enc reported an error
	goto encrypt_cleanup
)

@echo Generating MD_SIGN portion of metadata
gen_metadata -c %CONFIG_FILE% -phase1
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
	@echo ERROR: gen_metadata -phase1
	goto encrypt_cleanup
)

:: Now use ther Server priv key to sign the MD_SIGN inside of xml
openssl dgst -sha256 -binary -out temp.dgst md_sign.xml 
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
  @echo ERROR: openssl dgst reported an error
	goto encrypt_cleanup
)

:: The OEM signs the XML header
openssl rsautl -in temp.dgst -inkey %KEYDIR_OEM%/rsa_pr_key_golden_oem.pem -pkcs -sign -keyform PEM -out temp.dat
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
	@echo ERROR: openssl rasutl -in reported an error
	goto encrypt_cleanup
)

:: Convert signature of XML to base64
openssl enc -base64 -A -in temp.dat -out md_signature.dat
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
	@echo ERROR: openssl enc reported an error
	goto encrypt_cleanup
)

@echo Finished signing MD_SIGN now generating complete metadata
gen_metadata -phase2 -pad
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
	@echo ERROR: GenMetadata -phase2 reported an error
	goto encrypt_cleanup
)

:: now append the metadata to the start of the encrypted modem image
copy /B %MAGIC_NUMBER% + ssd_metadata.xml + %TEMP_ENC_IMAGE% %ENCRYPTED_IMAGE%
if NOT %ERRORLEVEL% EQU 0 (
	set ENCRYPT_ERROR=%ERRORLEVEL%
	@echo ERROR: Failed to generate %ENCRYPTED_IMAGE%
	goto encrypt_cleanup
)

@echo Created %ENCRYPTED_IMAGE%

:encrypt_cleanup
:: Unset encryption keys for security
set KEY=
set IV=

:: Clean up all our intermediate files for security purposes
del /q aes128.key
del /q aes128.iv
del /q %TEMP_ENC_IMAGE%
del /q iek_key.enc
del /q img_hash.dat
del /q md_signature.dat
del /q temp.dgst
del /q md_sign.xml
del /q ssd_metadata.xml
del /q temp.dat
del /q pub_key_id_oem.dat
del /q pub_key_id_dev.dat
del /q aes128_key.dat
del /q aes128_iv.dat
del /q gen_metadata.exe
del /q md_config.ini
del /q gen_rand.exe
del /q rand_seed_iv.dat
del /q rand_seed.dat
endlocal

popd

exit /B %ENCRYPT_ERROR%
