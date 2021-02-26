#DevCi automation
1. Jenkins: http://devci-telephony2:8080/job/Qcril/
2. Last build log: http://devci-telephony2:8080/job/Qcril/lastBuild/console

#Code coverage
1. gcov report: http://devci-telephony2:8080/job/Qcril/ws/output/index.html

#How to download the source ?
1. git clone https://github.qualcomm.com/yashdevs/qcril-hal

#How to build ?

## Android
1. cd qcril-hal
2. mm

## Visual Studio 2015
1. cd qcril-hal/test/
2. git clone https://github.qualcomm.com/yashdevs/googletest
3. cd ../
4. Double click on riltest.sln
5. Wait for Microsoft Visual 2015 to load the project.
6. Press ctrl+F5. Project should compile and run successfully.

## Linux host
1. Make sure that you have clang 3.9 and cmake 3.7 and lcov 1.12
2. cd qcril-hal/test/
3. git clone https://github.qualcomm.com/yashdevs/googletest
4. cd googletest/googletest/
5. cmake .
6. make
7. cd ../../../
8. make
9. ./riltest

#How to execute ?

## Android
1. adb root
2. adb remount
3. adb push out/target/product/msm8996/system/bin/riltest /data/
4. adb shell
5. cd /data/
6. ./testril

## How to fix qmi communication?
 riltest needs SeLinux domain defined as well as the uid and gid must be RADIO
 for it to communicate with qmi services.
 A gerrit would be shortly uploaded for riltest.te and init.qcom.rc

## Visual Studio 2015
1. Press ctrl+F5

## Linux host
1. ./riltest

##Unit test
Below is the current status of unit test.
<pre>
[==========] Running 15 tests from 2 test cases.
[----------] Global test environment set-up.
[----------] 9 tests from FrameworkUnitTest
[ RUN      ] FrameworkUnitTest.VerifyThatNoMessageIsRegisteredAtBootup
[       OK ] FrameworkUnitTest.VerifyThatNoMessageIsRegisteredAtBootup (0 ms)
[ RUN      ] FrameworkUnitTest.VerifyThatSingleModuleUnregistrationIsClean
[       OK ] FrameworkUnitTest.VerifyThatSingleModuleUnregistrationIsClean (0 ms)
[ RUN      ] FrameworkUnitTest.VerifyThatDuplicateMessageRegistrationIsClean
[       OK ] FrameworkUnitTest.VerifyThatDuplicateMessageRegistrationIsClean (0 ms)
[ RUN      ] FrameworkUnitTest.VerifyThatDoubleInitByModuleIsClean
[       OK ] FrameworkUnitTest.VerifyThatDoubleInitByModuleIsClean (0 ms)
[ RUN      ] FrameworkUnitTest.VerifyThatBroadcastReceiverModulesUnregistrationIsClean
[       OK ] FrameworkUnitTest.VerifyThatBroadcastReceiverModulesUnregistrationIsClean (0 ms)
[ RUN      ] FrameworkUnitTest.VerifyThatNonBroadcastReceiverModulesUnregistrationIsClean
[       OK ] FrameworkUnitTest.VerifyThatNonBroadcastReceiverModulesUnregistrationIsClean (0 ms)
[ RUN      ] FrameworkUnitTest.VerifyThatModemEndPointFactoryReturnCorrectTypeOfEndPoint
[       OK ] FrameworkUnitTest.VerifyThatModemEndPointFactoryReturnCorrectTypeOfEndPoint (0 ms)
[ RUN      ] FrameworkUnitTest.VerifyThatModemEndPointRefCoutingIsCorrect
[       OK ] FrameworkUnitTest.VerifyThatModemEndPointRefCoutingIsCorrect (0 ms)
[ RUN      ] FrameworkUnitTest.VerifyThatModemEndPointAreUnique
[       OK ] FrameworkUnitTest.VerifyThatModemEndPointAreUnique (0 ms)
[----------] 9 tests from FrameworkUnitTest (2 ms total)

[----------] 6 tests from BasicFeatureSanity
[ RUN      ] BasicFeatureSanity.TestUnregisteredMessageProcessing
[       OK ] BasicFeatureSanity.TestUnregisteredMessageProcessing (0 ms)
[ RUN      ] BasicFeatureSanity.TestRegisteredMessageProcessing
[       OK ] BasicFeatureSanity.TestRegisteredMessageProcessing (2 ms)
[ RUN      ] BasicFeatureSanity.TestEqualityOfModemEndPoints
[       OK ] BasicFeatureSanity.TestEqualityOfModemEndPoints (0 ms)
[ RUN      ] BasicFeatureSanity.TestDmsModemEndPointSetup
[       OK ] BasicFeatureSanity.TestDmsModemEndPointSetup (1 ms)
[ RUN      ] BasicFeatureSanity.TestVoiceModemEndPointSetup
[       OK ] BasicFeatureSanity.TestVoiceModemEndPointSetup (0 ms)
[ RUN      ] BasicFeatureSanity.TestUniquenessOfModemEndPoints
[       OK ] BasicFeatureSanity.TestUniquenessOfModemEndPoints (3 ms)
[----------] 6 tests from BasicFeatureSanity (6 ms total)

[----------] Global test environment tear-down
[==========] 15 tests from 2 test cases ran. (8 ms total)
[  PASSED  ] 15 tests.
</pre>

##How to run code coverage?
1. Make sure you have clang-3.9, gcov and lcov installed.
2. Run below command and firefox window would open with code coverage html page.

./buildCoverage

##How to run valgrind?
1. Make sure valgrind is installed on your system.
2. Run below command. The valgrind output would be present in valgrind.log

./runValgrind.sh

