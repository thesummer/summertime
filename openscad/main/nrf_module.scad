include <pin_header.scad>

pcb_length = 32.1;
pcb_width = 24.55;
pcb_height = 1.15;

module nrf_module()
{
    pcb_dim = [pcb_length, pcb_width, pcb_height];
    header_offset = [9,2,2.5];

    union()
    {
        color("grey") cube(pcb_dim);

        translate(header_offset) rotate([0,180,0]) header(2,9,2.0);
        translate(header_offset+[0,pcb_width-2*header_offset.y,0]) rotate([0,180,0]) header(2,9,2.0);
    }
}