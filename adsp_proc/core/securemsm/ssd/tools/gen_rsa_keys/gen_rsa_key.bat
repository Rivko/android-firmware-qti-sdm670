:: Copyright (c) 2010 by QUALCOMM Incorporated. All Rights Reserved 

@echo off
set local

:: Usage gen_rsa_key.bat <dir_name> 
:: the batch file expents a file 'rand_seed.dat'. The file should contain a large random numer

set ENCRYPT_ERROR=0

pushd .

set TOOLS_DIR=..
set GEN_RAND_DIR=%TOOLS_DIR%\gen_rand
set RAND_SEED=rand_seed.dat


%GEN_RAND_DIR%\gen_rand.exe > NUL 2>&1
if %ERRORLEVEL% EQU 9009 (
	@echo ERROR: gen_rand.exe not found in %GEN_RAND_DIR%
	set ENCRYPT_ERROR=1
	goto cleanup
)

copy %GEN_RAND_DIR%\gen_rand.exe /Y

gen_rand.exe 2048 > %RAND_SEED%


:: Generate RSA Private key in PEM format
openssl genrsa -out rsa_pr_key.pem 2048 -rand rand_seed.dat

:: Extract the Public key (in PEM) from private key (in PEM)
openssl rsa -pubout -in rsa_pr_key.pem -out rsa_pub_key.pem

:: Convert the PEM keys to DER keys
openssl rsa -outform DER -in rsa_pr_key.pem -out rsa_pr_key.der
openssl rsa -pubin -outform DER -in rsa_pub_key.pem -out rsa_pub_key.der

:: Convert the PEM RSA keys into PKCS8 PEM format
openssl pkcs8 -topk8 -inform DER -outform DER -nocrypt -in rsa_pr_key.der -out rsa_pkcs8_pr_key.der

:: Create the Public key Hash in binary format
openssl dgst -sha256 -binary -out rsa_pub_key_id.dat rsa_pub_key.der

:: Create the Public key Hash in base64 format
openssl dgst -sha256 -binary rsa_pub_key.der | openssl enc -base64 -out rsa_pub_key_id.b64


:: Move the keys & Pub key ID to a seperate directory
del /q gen_rand.exe
del /q rand_seed.dat
mkdir %1
move *.der %1\.
move *.pem %1\.
move *.dat %1\.
move *.b64 %1\.
goto endKeys


:cleanup
del /q gen_rand.exe
del /q rand_seed.dat
:endKeys