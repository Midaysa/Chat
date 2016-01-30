proyect: 
	cd cliente; make client	&& \
	cd ..; cd chat; make chat && \
	cd ..; cd servidor; make server

clean:
	cd cliente; make clean	&& \
	cd ..; cd chat; make clean && \
	cd ..; cd servidor; make clean 

