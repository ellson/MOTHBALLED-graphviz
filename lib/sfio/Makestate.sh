if test "`cat Make.state`" != "$1"
then	rm *.o Stdio_b/*.o Stdio_s/*.o Sfio_f/*.o
fi >/dev/null 2>&1
echo "$1" > Make.state
exit 0
