PG_HEADERS="C:\\Program Files\\Point Grey Research\\FlyCapture2\\include"
JNI_HEADER="C:\\Program Files\\Java\\jdk1.7.0_79\\include"
JNI_HEADER_WIN32="C:\\Program Files\\Java\\jdk1.7.0_79\\include\\win32"
JNI_HEADER_GENERATED_PATH="./"
DLL_OUTPUT_FILE=wflyclient\lib\wpointgrey.dll

all:wflypointgrey.class wflypointgrey.h wpointgrey.dll client

wflypointgrey.h:wflypointgrey.class
	javah -d wflypointgreyimpl\src\ -classpath wflypointgrey/bin WPointGreyApp
	
wflypointgrey.class:
	javac -d wflypointgrey/bin -sourcepath wflypointgrey/src wflypointgrey/src/*.java
	
wpointgrey.dll:
		gcc -m64 -Wl,--add-stdcall-alias -I$(JNI_HEADER) -I$(JNI_HEADER_WIN32) -I$(PG_HEADERS) -L$(JNI_HEADER_GENERATED_PATH) -lFlyCapture2_C -shared -o $(DLL_OUTPUT_FILE) wflypointgreyimpl\src\wpointgrey.c

client:
	javac -d wflyclient/bin -classpath wflypointgrey/bin;wflyclient/bin  -sourcepath wflyclient/src/ wflyclient/src/*.java

package: clientjars wrapperjars
	jar cvf
clientjars:
		jar cvf
wrapperjars:
	jar cvf
run:
	java -Djava.library.path=wflyclient/lib -classpath wflypointgrey/bin;wflyclient/bin ClientJavaPointGray

clean:
	del /S /Q wflyclient/bin/* wflypointgrey/bin/* wflyclient/lib/*