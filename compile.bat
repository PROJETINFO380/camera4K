    del *.o *.class *.log wpointgrey.dll
	
	javac WPointGreyApp.java
	
	javah WPointGreyApp
	
	gcc -m64 -Wl,--add-stdcall-alias -I"%JAVA_HOME%\include" -I"%JAVA_HOME%\include\win32" -I"C:\dev\fc\include" -L"./" -lFlyCapture2_C -shared -o wpointgrey.dll wpointgrey.c

	java WPointGreyApp