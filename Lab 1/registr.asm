.model small
.stack 100h

.data

WriteError db "Write error!",0Dh,0Ah,'$'
ReadError db "Read error!",0Dh,0Ah,'$'
DBIN db 'K'
DBOUT db 'E'
           
.code
           
Init_COM1 proc  
    mov al,80h   
    mov dx,3FBh
    out dx,al
    
    mov dx,3F8h         ;COM1
    mov al,00h          ;DIM
    out dx,al
    mov al,0dh          ;DLL
    mov dx,3F9h         ;115200
    out dx,al
    
    mov dx,3FCh         ;modem control
    mov al,00001011b    
    out dx,al  
    
    mov al,0000011b    ;8 bytes
    mov dx,3FBh         ;LCR
    out dx,al 
    
    mov dx,3F9h         ;DIM
    mov al,001001b
    out dx,al   
    
    ret
Init_COM1 endp

NoWRite proc
   mov ah,9
   mov dx,offset WriteError
   int 21h 
   jmp error
   ret 
NoWRite endp

NoRead proc
   mov ah,9
   mov dx,offset ReadError
   int 21h 
   jmp error
   ret 
NoRead endp

Write proc
    mov dx,3F8h
    mov al,DBIN     ;input byte
    out dx,al 
    xor al,al
    mov dx,3FDh      ;sostoyanie  line
    in al,dx
    test al,10h       ; check overrun
    jnz NoWRite
    ret
Write endp

Read proc 
    mov cx,7
    mov ah, 86h
    int 15h           ;7 msec wait
    mov dx,2F8h       ;COM2
    in al,dx
    mov DBOUT,al      
    xor al,al
    mov dx,2FDh         ;sostoyanie line LSR
    in al,dx
    test al,10b
    jnz NoRead
    ret
Read endp


start:
    mov ax,@data
    mov ds,ax
    call Init_COM1
    call Write
    call Read
    mov ah,02h
    mov dl,DBOUT  
    int 21h  
    error:         ;error output
    mov ax,4C00h
    int 21h
    ret
   
end start