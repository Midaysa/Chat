proyect: 
	cd cliente; make client	&& \
	cd ..; cd servidor; make server

clean:
	cd cliente; make clean	&& \
	cd ..; cd servidor; make clean
	

