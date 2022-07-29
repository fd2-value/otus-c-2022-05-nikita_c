    bits 64 											
    extern malloc, puts, printf, fflush, abort, free			
    global main											

    section   .data									
empty_str: db 0x0										
int_format: db "%ld ", 0x0							
data: dq 4, 8, 15, 16, 23, 42								
data_length: equ ($-data) / 8							

    section   .text
;;; print_int proc
print_int:								
    mov rsi, rdi						
    mov rdi, int_format					
    xor rax, rax						
    call printf							

    xor rdi, rdi						
    call fflush							

    ret

;;; p proc
p:										
    mov rax, rdi						
    and rax, 1							
    ret

;;; add_element proc
add_element:							
    push rbp							
    push rbx							
									
    mov rbp, rdi						
    mov rbx, rsi						

    mov rdi, 16							
    call malloc							
    test rax, rax					
    jz abort						

    mov [rax], rbp						
    mov [rax + 8], rbx					

    pop rbx								
    pop rbp							

    ret

;;; m proc
m:									
    test rdi, rdi						
    jz outm
						
    push rbx							
										
    mov rbx, rdi						

    mov rdi, [rdi]											
    call print_int

    mov rdi, [rbx + 8]				
    call m								

    pop rbx								
	
outm:							
    ret

;;; f proc
f:										
    mov rax, rsi						

    test rdi, rdi						
    jz outf								

    push rbx						
    push r12

    mov rbx, rdi						
    mov r12, rsi						

    mov rdi, [rdi]						
    call p
    test rax, rax					
    jz z
									
    mov rdi, [rbx]						
    mov rsi, r12						
    call add_element					
    mov rsi, rax					
    jmp ff							
										
z:										
    mov rsi, r12						
	
ff:										
    mov rdi, [rbx + 8]			
    call f								

    pop r12
    pop rbx

outf:
    ret

; очистка памати
global_free:
	; пока список не пуст
	test rdi, rdi
	jz out_global_free
	; следующий элемент временно сохраняем в регистр rbx
	mov rbx, [rdi + 8]	
	; очистка того, на что указывает rdi
	call free
	; возвращаем адрес следующего элемента списка в rdi
	mov rdi, rbx
	jmp global_free

out_global_free:
	ret


;;; main proc
main:
    push rbx

    xor rax, rax
    mov rbx, data_length				
adding_loop:							
    mov rdi, [data - 8 + rbx * 8]		
    mov rsi, rax					
    call add_element					
    dec rbx								
    jnz adding_loop					

    mov rbx, rax						

    mov rdi, rax				
    call m								

    mov rdi, empty_str					
    call puts							
	
    mov rdx, p					
    xor rsi, rsi						
    mov rdi, rbx						
    call f								

    mov rdi, rax						; rax содержит адрес начала нового списка с уже отфильтрованными элементами
    mov rbp, rax						; адрес из rax сохраняем в текущем стеке
    call m								
		
	mov rdi, rbx 						; освобождает память из первого списка
	call global_free	
	
	mov rdi, rbp 						; освобождает память из списка полученного после работы f
	call global_free
		
    mov rdi, empty_str					
    call puts							
	
    pop rbx

    xor rax, rax
    ret
