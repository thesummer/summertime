
$fn = 100;

//button("red", 5);

ring_d1 = 18;
ring_d2 = 14;
ring_h  =  2;
button_d = 13;
button_h = 6.3;


conn_length = 5.5;
conn_width  = 2.0;
conn_height = 2.0;
conn_dim = [conn_width, conn_height, conn_length];

length = 24;
body_length = length - conn_length;
body_d      = 16;

screw_h = 7;
screw_d = ring_d1;

module button(button_color = "green", screw_offset = 1.5)
{
    translate([0,0,body_length])
    {
        color("darkgrey") cylinder(h=ring_h, d1=ring_d1, d2=ring_d2);
        color(button_color) cylinder(h=button_h, d=button_d);
    }
    color("darkgrey") cylinder(h=body_length, d=16);

    translate([0,0,body_length-screw_h-screw_offset]) difference()
    {
        color("darkgrey") cylinder(h=screw_h, d=screw_d);
        color("darkgrey") cylinder(h=body_length, d=16);
    }

    translate([2.5,0,-conn_length/2]) color("gold") cube(conn_dim, center=true);
    translate([-2.5,0,-conn_length/2]) color("gold") cube(conn_dim, center=true);
    translate([3,5,-conn_length/2]) color("gold") cube(conn_dim, center=true);
    translate([-3.5,5,-conn_length/2]) color("gold") cube(conn_dim, center=true);
}