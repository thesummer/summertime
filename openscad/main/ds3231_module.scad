include <pin_header.scad>

pcb_length = 28.6;
pcb_width  = 17.9;
pcb_height = 1.5;
pcb_dim = [pcb_length, pcb_width, pcb_height];

ic_length = 10.4;
ic_width  =  7.5;
ic_height = 4.2 - pcb_height;
ic_dim = [7.5, 10.4, ic_height];

module ds3231_module()
{
    header_offset = [1.27,2.54*7/2, -5];

    difference()
    {
        union()
        {
            color("darkred") cube(pcb_dim);
            translate([5.7,0.5,pcb_height]) color("grey") cube(ic_dim);
        }
        translate(header_offset) header(7,1,2.54);
    }
}