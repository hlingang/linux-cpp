SECTIONS
{
    security.text : ALIGN(4096) {
    __start_security = .;
    __start_security_text = .;
    __security_text_start = .;
    KEEP(*(security.text))          
    __stop_security_text = .;
    __security_text_stop = .;
    }
} 
INSERT AFTER .text

SECTIONS
{
   security.data : ALIGN(4096) {
    __start_security_data = .;
    KEEP(*(security.data))     
    __stop_security_data = .;
    __stop_security = .; 
    }
} 
INSERT AFTER .data  