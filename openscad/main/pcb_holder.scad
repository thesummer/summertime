include<nrf_module.scad>

nrf = getNrfDimensions();

rotate([90,0,0]) pcb_holder([nrf.x+0.25, 12, nrf.z+0.25], 2.0, 2, 6, nrf.z+9);

module pcb_holder(pcb_dim, thickness, groove_depth, groove_height, height)
{
    difference()
    {
        cube([2*thickness+pcb_dim.x, pcb_dim.y, height]);
        translate([thickness+groove_depth,0,thickness]) cube([pcb_dim.x-2*groove_depth, pcb_dim.y, height-thickness]);
        translate([thickness,0,groove_height]) cube(pcb_dim);
    }

}