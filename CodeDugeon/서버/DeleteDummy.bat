#echo off
rmdir	/S /q ipch
rmdir	/S /q .vs
rmdir	/S /q Debug
rmdir	/a /q .vc
rmdir	/S /q .\Core\Default\Debug
rmdir	/S /q .\Core\Default\x64
rmdir	/S /q .\App\Default\Debug
rmdir	/S /q .\App\Default\x64
rmdir	/S /q .\BufferLib\Debug
rmdir	/S /q .\BufferLib\x64
del		*.VC.db