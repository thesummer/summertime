include <pin_header.scad>

pcb_length = 18.5;
pcb_width  = 10.4;
pcb_height =  1.1;
pcb_dim = [pcb_length, pcb_width, pcb_height];

header_offset = [1.27, 2*2.54,-5];

ic_length = 7.2;
ic_width  = 6.65;
ic_height = 3.0;
ic_offset = [3.15+2.54, 0.3, pcb_height];

union()
{
    difference()
    {
        color("darkred") cube(pcb_dim);
        translate(header_offset) header(4,1,2.54);
    }
    translate(ic_offset) color("silver") cube([ic_length, ic_width, ic_height]);
}