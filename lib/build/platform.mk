ifeq ($(OS),Windows_NT)
	EXE:=.exe
	PLATFORM:=win32
else
	PLATFORM:=posix
endif
