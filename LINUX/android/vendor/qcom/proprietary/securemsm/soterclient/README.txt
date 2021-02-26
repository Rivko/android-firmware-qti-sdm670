soter client
======================

This test app present examples illustrating the use of SOTER ATTK provision APIs published in the keymaster1.h header file for the Keymaster HAL library.

The compliment to this HLOS client is the "keymaster" that resides and runs on the secure trustzone domain. The soter client communicates with the keymaster TA/HAL via issuing various commands..

***********************************************************************************
                  PLEASE SEE THE EXAMPLE RUN BELOW
***********************************************************************************

---------------------------------------------------------
Usage: soter_client [CMD]
CMD can be:
  1: Generate ATTK key pair
  2: Verify ATTK key pair
  3: Export ATTK public key
  4: Get device ID

 - adb push <BUILD>out/target/product/<TARGET>/system/bin/soter_client /system/bin
 - su
 - Run soter_client:
  do 'soter_client [CMD]'
---------------------------------------------------------
shell@msm8996:/ $ su
su
root@msm8996:/ # soter_client 1
soter_client 1
pass
root@msm8996:/ # soter_client 2
soter_client 2
pass
root@msm8996:/ # soter_client 3
soter_client 3
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAvdZS9ClJ6xwc4ID4PrQE
zZA10IKl+jdj12QjilQDzg3uOj7FcvW8pNKyPJB9Y0FgYgKl+9B33595YPG0eiSg
Llj0BgGG1HHQXh8q822O3wUuRaDrklh9mzfiWjyIyZ0WbBbxZNe/BthkIocihrgT
DyNg0hBZa7nraEM/Vqrt4KI3cqfA9LlyfhcNSIckOWIXs5SiXw8c1lc2/G+SQ9zN
FBnu1vMnoyEDbcH1gr814Xu+hKBjiTEfMdtUQqCwkAFxdD393fJZl/5uup0YyveQ
+zjJhLcT60wKat0c+xfbdwh53pOXyZSBapRlO2zgPgy/VakLf9eOikNw5x+PXsot
KQIDAQAB
-----END PUBLIC KEY-----
root@msm8996:/ # soter_client 4
soter_client 4
00000000e170942053f0841e4538554d
