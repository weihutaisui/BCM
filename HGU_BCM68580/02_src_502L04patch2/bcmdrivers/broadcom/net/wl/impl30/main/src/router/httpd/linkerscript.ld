SECTIONS
{
    ej_handlers : {
        PROVIDE(__start_ej_handlers = .);
        *(ej_handlers)
        PROVIDE(__stop_ej_handlers = .);
    }
}
INSERT AFTER .data;
