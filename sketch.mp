http://www.lowtek.com/sockets/select.html
http://www.beej.us/guide/bgnet/output/html/multipage/selectman.html
http://digi-cron.com:8080/programmations/c/lectures/5-fifos.html

server
    crear pipe (nominal) 'conexiones_nuevas'
    crear diccionario de usuarios
    
    while (1)
        
        revisar el pipe 'conexiones_nuevas'
        si existe una nueva solicitud de conexion:
            leer del pipe 'conexiones_nuevas' los id's de los pipes del cliente
            agregar informacion del nuevo usuario al diccionario
        
        
        
    
client
    crear pipes (nominales) 'entrada' y 'salida'
    escribir los id's de los pipes creados, hacia el pipe 'conexiones_nuevas'
        del servidor
    abrir los pipes creados