/**
asd
*/

book_l = 190;
book_w = 150;

draw_book(show_bot=false, show_top=false, show_hardware=false, tilt_brackets= true, hinge_stuff=false, cover=false);



module draw_book(show_bot=true, show_top=true, show_hardware=true, tilt_brackets=true, hinge_stuff=true, cover=true) {
    if(show_bot) {
        difference(){
       cube([book_w, book_l, 3 ]);
            //Powerbank Kabel loch
            //translate([77.5, 129-126, -1])
            //cube([15,10,100]);
            
            // Löcher für Sensoren
            translate([18/2+2, 65, -0.1])
            cylinder(r=5, h=205.01,$fn=60, center = true);
        translate([18/2+2, 15+65, 0])
            cylinder(r=4.5, h=225.01,$fn=60, center = true);
            
            translate([18/2+2, 5+105, -0.1])
            cylinder(r=5, h=205.01,$fn=60, center = true);
        translate([18/2+2, 20+105, 0])
            cylinder(r=5, h=225.01,$fn=60, center = true);
                       
            translate([18/2+book_w-18, 65, -0.1])
            cylinder(r=5, h=205.01,$fn=60, center = true);
        translate([18/2+book_w-18, 15+65, 0])
            cylinder(r=5, h=225.01,$fn=60, center = true);
            
            translate([18/2+book_w-18, 5+105, -0.1])
            cylinder(r=4.5, h=205.01,$fn=60, center = true);
        translate([18/2+book_w-18, 20+105, 0])
            cylinder(r=5, h=225.01,$fn=60, center = true);
            
        }
       //Seiten 
       cube([book_w, 2, 40]);
        
       translate([0, book_l, 0]) {
            difference(){
                cube([book_w, 2, 40]);
                translate([book_w/2+22, -0.01, 10])
                cube([30, 2.02, 20]); 
            }
            
       

       }
       cube([2, book_l, 40]);
       translate([book_w, 0, 0]) {
           cube([2, book_l+2, 40]);
        }
        
        //Streben für die Unterseite/Boden/Rückseite
        
        //funkt nicht...
        //translate([0, 5, 2])
        //rotate([0, 0, 360-39.4 ])
        //color("orange")
        //    strebe();
        l_c = sqrt((book_w-2)^2+(book_l-2)^2);
        difference() {
        color("orange")
        translate([-5,2.5,1])
        rotate([0,0,-asin((book_w-2)/l_c)])
        cube([10, l_c+7, 3]);
            translate([-5, -5, -5])
            cube([6, 20, 50]);
            translate([-5, -5, -5])
            cube([20, 6, 50]);
            
            translate([+book_w, book_l+24, -5])
            rotate([0,0,270])
            cube([100, 20, 50]);
            translate([+100, book_l+1, -5])
            rotate([0,0,0])
            cube([202, 600, 500]);
            
        }
        
        difference() {
        color("orange")
        translate([-5+book_w+1,-2,1])
        rotate([0,0,asin((book_w-2)/l_c)])
        cube([10, l_c+7, 3]);
            translate([book_w, -5, -5])
            cube([6, 20, 50]);
            translate([book_w-20, -59, -5])
            cube([200, 60, 10]);
            
            translate([0-18.5, book_w+50, -5])
            rotate([0,0,270])
            cube([100, 20, 50]);
            translate([+-7, 173+18, -5])
            rotate([0,0,0])
            cube([90, 20, 50]);
            
        }
        
        //Verbindung für Kipp-sensor stehendes buch
         translate([book_w/2+5, book_l + 10, 5]) {
        color("orange") {
            


            //Verbindung von Boden zu
            translate([25-60,-40, 9]){
                translate([0, -10, -14])
            cube([25, 20, 15]);

            rotate([270, 270, 0]){
            hx_sensor_cube(l=25,w=12, h=10);
            }}

        }}
        
        //weitere nicht-diagonale Streben
        color("orange")
        translate([2+18, 60+10, 1])
            cube([book_w-36, 10, 3]);
        color("orange")
        translate([2+18, 100+10, 1])
            cube([book_w-38, 10, 3]);
        color("orange")
        translate([120-5, 2, 1])
            cube([10, book_l, 3]);
        color("orange")
        translate([31-5, 2, 1])
            cube([10, book_l, 3]);
        
        //Stützen für arduino
        translate([85-3.5*2.54-1+8, book_l-8.5*2.54*2-29.5, 2]) {
        color("red")
//            difference(){
//            cube([7*2.54+4, 17*2.54+4, 10]);
//            translate([0-1,2+2+0.45,-0.1])
//                cube([6,17*2.54-4-0.9,11]);
//            translate([7*2.54-1.9,2+2+0.45,-0.1])
//                cube([6,17*2.54-4-0.9,11]); 
//                //Bohrungen
//             translate([3.25,3.25,-0.1])
//                cylinder(d=1, h=50, $fn=20);
//            translate([3.25+15.25,3.25,-0.1])
//                cylinder(d=1, h=50, $fn=20); 
//            translate([3.25,3.25+40.7,-0.1])
//                cylinder(d=1, h=50, $fn=20);
//            translate([3.25+15.25,3.25+40.75,-0.1])
//                cylinder(d=1, h=50, $fn=20);
                translate([0,0,0])
            cube([7*2.54+4, 17*2.54+4, 3]);
   
//            }
        }
        //Stützen für sensoren

        
        translate([2, 55, 2]) {
        color("orange") {
//            cube([18, 30, 3]);
            translate([0, 5,0])
                hx_sensor_cube();
        }}
        
        translate([book_w -18, 100, 2]) {
        color("orange") {
//            cube([18, 30, 3]);
            translate([0, 5,0])
                hx_sensor_cube();
        }}    
        
        translate([book_w -18, 55, 2]) {
        color("orange") {
//            cube([18, 30, 3]);
            translate([0, 5,0])
                hx_sensor_cube();
        }}
        
        translate([2, 100, 2]) {
        color("orange") {
//            cube([18, 30, 3]);
            translate([0, 5,0])
                hx_sensor_cube();
        }}
        
        
        
        //Powerbank-stützen
        pb_h = 12;
        translate([book_w/2+30.5, 10, 0]) {            
            color("orange") {
                cube([2,86,pb_h]);
                translate([-63, -2, 0])
                    cube([65, 2, pb_h]);
                translate([-63, 0,0])
                    cube([2, 86, pb_h]);
                translate([-63, 86, 0])
                    cube([65,2,6]);
                rotate([0,0,90])
                cube([86, 61, 4]);
            }
            
        }       
          
        //HX711 Stützen
               //HX711
        translate([120, 80, 3])
        rotate([0,0,90])
            hx_ground();
        
        translate([120, 120, 3])
            rotate([0,0,270])
            hx_ground();
        
        translate([31, 120, 3])
            rotate([0,0,270])
            hx_ground();
        
        translate([31, 80, 3])
            rotate([0,0,90])
            hx_ground();
        
        //5. Sensor für Buch kippen        
        translate([(book_w/2)-12, 122, 3])
            rotate([0,0,90])
                hx_ground();        
    } //bot    
    
    if(show_top) {
        translate([0, 0, 42])
        {
            difference() {
                color("orange")
                cube([book_w, book_l, 3 ]);
                translate([25, 20, -0.1])
                cube([100, 60, 3.2]); 
                translate([25, 110, -0.1])
                cube([100, 60, 3.2]); 
                
                
                //Löcher für Sensoren
             translate([18/2+2, 5+5, -0.1])
            cylinder(r=4.5, h=205.01,$fn=60, center = true);
            translate([18/2+2, 20+5, 0])
            cylinder(r=4.5, h=225.01,$fn=60, center = true);
            
            translate([18/2+2, 165, -0.1])
            cylinder(r=4.5, h=205.01,$fn=60, center = true);
        translate([18/2+2, 15+165, 0])
            cylinder(r=4.5, h=225.01,$fn=60, center = true);
                       
            translate([18/2+book_w-18, 5+5, -0.1])
            cylinder(r=4.5, h=205.01,$fn=60, center = true);
        translate([18/2+book_w-18, 20+5, 0])
            cylinder(r=4.5, h=225.01,$fn=60, center = true);
            
            translate([18/2+book_w-18, 165, -0.1])
            cylinder(r=4.5, h=205.01,$fn=60, center = true);
        translate([18/2+book_w-18, 15+165, 0])
            cylinder(r=4.5, h=225.01,$fn=60, center = true);
            }
        } 
        
        
        
         translate([0, 0, 47]) {
             // Streben 
             translate([6,30, -10])
                cube([10,130, 5]);
             translate([136,30, -10])
                cube([10,130, 5]);
             translate([17,5, -10])
                cube([120,10, 5]);
             translate([17,175, -10])
                cube([120,10, 5]);
             

         if(hinge_stuff) {
         translate([0, 0, -2])
            hinge ();
         //Für Schließen; einklemmen
         color("darkblue") {
         translate([book_w-7, 60, -2])
         difference(){
            cube([7,100, 7]);
             translate([-0.1, -0.1, -0.25+3])
             cube([3, 120, 2.5]);
         }}}
        }
        
        //Verbindung zu Sensoren
        //cube x: 2 weniger als unter sensor, da nicht mit außenwand verbunden werden soll
        
        translate([2, 5, 2]) {
        color("orange") {
            translate([1.5, 0, 12.5+17.5])
            hx_sensor_cube(h=10, hole_r=2.1, w=15);
        }}
        
        translate([130, book_l, 2]) {
        color("orange") {
            translate([3.5, -30, 12.5+17.5])
            hx_sensor_cube(h=10, hole_r=2.1, w=15);
        }}
    
        translate([130, 5, 2]) {
        color("orange") {
            translate([3.5, 0,12.5+17.5])
            hx_sensor_cube(h=10, hole_r=2.1, w=15);
        }}
        
        translate([2, book_l, 2]) {
        color("orange") {
            translate([1.5,-30,12.5+17.5])
            hx_sensor_cube(h=10, hole_r=2.1, w=15);
        }}
    }//top
    
     //Coverseite
        if(cover) {
         translate([0, 0, 47]) {
             

             color("darkred") {
                 //Seite
             translate([10, 0, 2])
                cube([book_w-15, book_l, 1 ]);
                 //gelenke für Drehbewegung
                drehgelenk();
                 translate([0,50,0])
                    drehgelenk();
                 translate([0,100,0])
                    drehgelenk();
                 translate([0,150])
                    drehgelenk();
         }}
        }
    //für oben der sensor; aus regal kippen   
   if(tilt_brackets) 
        translate([book_w/2, book_l + 10, 5]) {
        color("orange") {
            difference(){
            cube([80, 2, 30]);
             rotate([90,90,0]) {
                translate([-15,30,-0.01]) {
                translate([0, 0, -0.1])
            cylinder(r=3, h=205.01,$fn=60, center = true);
                translate([0, 15, 0])
            cylinder(r=3, h=225.01,$fn=60, center = true);
            }}}
            
            translate([25,0, 21])
                //cube([18, 15, 10]);
                rotate([90,90,0])
                hx_sensor_cube(l=25,w=12,hole_r=2.1, h=15);
        }}
 
    if(show_hardware) {
        //Arduino
        translate([86+8, book_l-49 , 5.03]) {
            //cube( [45,21,0], center=true );
            //cube( [20,13,12], center=true );

            rotate([0,0,90]) {
                      arduino_nano( hull=false, hull_headers_pin_length=0 );

        }}
        
        //HX711
        translate([120, 80, 3])
        rotate([0,0,90])
            hx711_green();
        
        translate([120, 120, 3])
            rotate([0,0,270])
                hx711_green();
        
        translate([31, 120, 3])
            rotate([0,0,270])
            hx711_green();
        
        translate([31, 80, 3])
            rotate([0,0,90])
                hx711_green();
                
                
        //5. Sensor für Buch kippen        
        translate([(book_w/2)-12, 120, 3])
            rotate([0,0,90])
                hx711_green();
        
        //HX711-Sensors
        hx_h = 20;
        // hx ist zentriert -> halbe breite zu z adieren
        translate([11, 45, 5.75+hx_h])
        rotate([0, 0, 270])
            load_cell_TAL5kg();
        
        translate([book_w -9, book_l-45, 5.75 + hx_h])
            rotate([0, 0, 90])
            load_cell_TAL5kg();
            
        translate([book_w -9, 45,  5.75 +hx_h])
        rotate([0, 0, 270])
            load_cell_TAL5kg();
        
        translate([11, book_l-45, 5.75 + + hx_h])
            rotate([0, 0, 90])
            load_cell_TAL5kg();
        
    //lezte für oben; kippen messenw    
        translate([book_w/2+10, book_l-20+8.25, 20])
            rotate([90, 0, 0])
            load_cell_TAL5kg();    
        
        // Powerbank
        translate([book_w/2+30.5, 10, 3])
            rotate([0, 0, 90])
                powerbank();

    }   
}


module powerbank() {
    color("white")
        cube([86, 61, 13.8]);
            translate([43, 30, 13.8])
                electronics_label("Powerbank", letter_size=8);
}

// From https://en.wikibooks.org/wiki/OpenSCAD_User_Manual/Primitive_Solids
 module prism(l, w, h){
       polyhedron(
               points=[[0,0,0], [l,0,0], [l,w,0], [0,w,0], [0,w,h], [l,w,h]],
               faces=[[0,1,2,3],[5,4,3,2],[0,4,5,1],[0,3,4],[5,2,1]]
               );
       
       
       }

//Strebe genau angepasst für den Boden des Buches in der festgelegten Maße. Maße wurden Anhand von sin und Dreicksbildung berechnet. FUnktioniert aber leider nicht :D
       
module strebe(high=3, width=10, book_l=book_l, book_w=book_w) {
    
    s_book_l = book_l-2;
    s_book_w = book_w-2;
    alpha = asin(book_w/book_l);
    b = width * sin(alpha);
    d = b * sin(alpha);
    h = sqrt(b^2 +d^2) ;
    
    points= [
        // unterseite
        [0, 0, 0],                          // 0
        [width, 0, 0],                      // 1
        [0, sqrt(s_book_w^2+s_book_l^2)-2*h, 0],        // 2
        [width, sqrt(s_book_w^2+s_book_l^2)-2*h, 0],    // 3
        [width/2, sqrt(s_book_w^2+s_book_l^2)-h, 0 ],   // 4
        [width/2, -h, 0],                    // 5
        // Oberseite
        [0, 0, high],                       // 6
        [width, 0, high],                   // 7
        [0, sqrt(s_book_w^2+s_book_l^2)-2*h, high],     // 8
        [width, sqrt(s_book_w^2+s_book_l^2)-2*h, high], // 9
        [width/2, sqrt(s_book_w^2+s_book_l^2)-h, high], // 10
        [width/2, -h, high]                  // 11

    
        // 2. Dreieckspunkt
      ];
    
    faces = [
        [0,2,4,3,1,5],     // bot
        [6,8,10,9,7,11],    // top
        [0, 2, 8, 6],    // quader side1
        [1, 3, 9, 7],    // quader side2
        [0, 6, 11, 5],   // dreieck1
        [1, 7, 11, 5],   // dreieck2
        [2, 4, 10, 8],   // dreieck3
        [3, 4, 10, 9]    // dreieck4
    ];
    translate([0,h,0])
    translate([-5, 2, 2])
    rotate(-alpha)
    polyhedron(points, faces);
}
        hx_sens_topCube = [18, 22, 10];

module hx_sensor_cube(h=17, hole_r=2.6, w=18, l=25) {  
    difference(){   
    cube([w, l, h]);
        translate([w/2, 5, -0.1])
            cylinder(r=hole_r, h=205.01,$fn=60, center = true);
        translate([w/2, 20, 0])
            cylinder(r=hole_r, h=225.01,$fn=60, center = true);
        
        
        
        }
    }
    
module hx711_green(){
    l = 33.6;  w=20.5;  h=1.6; xh= 1/2; zh=5.05;
    
    difference() {
      union() {
          // main pcb
          color( "darkgreen" )
          translate( [0,0,h/2] )
            cube( [l,w,h], center=true );
          // hx 711 chip
          color( "black" )
            translate( [0,0,h+0.5] )  
              cube( [4.5, 10.0, 1.0], center=true );

            color( "black") 
              translate( [-xh,0,h+zh/2] )
                cube( [2.54, 5*2.54, zh], center=true );
            color( "black") 
              translate( [+xh,0,h+zh/2] )
                cube( [2.54, 5*2.54, zh], center=true );
            
        }
        
        //hard-codiert und nicht variable...muss wieder angepasst werden
       for( i=[-1.5,-0.5,0.5,1.5,] ) {
      dy = i * 2.54;
      translate([15,0,0])
      translate( [-xh,dy,-0.5] ) 
        cylinder( d=1.0, h=h+zh+1, center=false, $fn=20 );
    } 
    
      translate([15-xh,w/2-3,0])  
        cylinder( d=3.0, h=h+zh+1, center=false, $fn=20 );
      translate([15-xh,-w/2+3,0])  
        cylinder( d=3.0, h=h+zh+1, center=false, $fn=20 );


    } //diff
    
    translate( [2*2.54, 0, h+0.5] ) rotate( [0,0,90] ) 
  	linear_extrude(height = 0.2) {
	  	text( "HX 711", size = 3, font = "Liberation Sans", 
            halign = "center", valign = "center", $fn = 16);
	  }
} //hx711 green

module hx_ground() {
    l = 33.6+2;  w=20.5+2;  h=2; xh= 1/2; zh=5.05;
    difference(){
        color("orange")
    cube([l,w,h], center=true);
        
     for( i=[-1.5,-0.5,0.5,1.5,] ) {
      dy = i * 2.54;
      translate([15,0,0])
      translate( [-xh,dy,-2] ) 
        cylinder( d=1.0, h=h+zh+1, center=false, $fn=20 );
    }    
        
    translate([15-xh,w/2-4,-2])  
        cylinder( d=2.8, h=h+zh+10, center=false, $fn=20 );
      translate([15-xh,-w/2+4,-2])  
        cylinder( d=2.8, h=h+zh+10, center=false, $fn=20 );
    }
}
  
//Für COverseite; halterung, an dem die Seite angeclipppt werden kann
module hinge (){
    color("blue") {
        for( i=[10,60,book_l-80,book_l-30] ) {
            translate([2.5,i,4.5])
            rotate([270,0,0])
            cylinder(d=5, h=20, $fn=200);
        }
        }
        
    color("darkblue") {
        translate([0,0,0])
            cube([5,10,7]);
        translate([0,30,0])
            cube([5,30,7]);
        translate([0,80,0])
            cube([5,30,7]);
        translate([0,130,0])
            cube([5,30,7]);

        translate([0,book_l-10,0])
            cube([5,10,7]);
        }
}
    
module drehgelenk() {
    translate([2.5,11,2.5]) {
        rotate([270,0,0])
            difference() {
                union(){
                cylinder(d=7, h=18, $fn=200);
                translate([0,-1.5,0])
                    cube([10,3,18]);
                }
                translate([0,0,-1]) 
                    cylinder(d=5.5, h=40, $fn=200);
                translate([-3.5,0,-1])
                cylinder(d=5.5, h=40, $fn=200);     
                }
    }
    
}


//loadCell
/** 
 * load cell 5 kg
 * selbst vermessen; ursprung von 220
 */                  
module load_cell_TAL5kg(
  load_cell_color = [0.8, 0.8, 0.8],
  sensor_size = [ 24,12,1 ],
  sensor_color = [1,1,1],
  m4_screw_length = 0,
  m4_top_screw_head_length = 0,
  m4_bot_screw_head_length = 0,
  m5_screw_length = 0,
  m5_top_screw_head_length = 0,
  m5_bot_screw_head_length = 0,
)
{
  ll = 80; ww = 12.7; hh = 12.7; hhgage = 1;
  
  
  difference() {
    union() {
      // main body
      color( load_cell_color )
        cube( [ll, ww, hh], center=true );
      
      // strain gages
      color( sensor_color ) 
      for( dz=[-hh/2-sensor_size[2]/2, hh/2+sensor_size[2]/2] ) 
        translate( [0,0,dz] ) 
          cube( [sensor_size[0], sensor_size[1], sensor_size[2]], center=true );
      
      // cable routing on one side
      color( sensor_color ) 
        translate( [7+10,ww/2,0] )
          cube( [14,1,hh], center=true );
    }

    // cylindrical cutouts
    for( dx=[-4,4] ) 
      translate( [dx,0,0] ) rotate( [90,0,0] ) 
        cylinder( d=12.0, h=20+ww+1, center=true, $fn=50 );
   
    // 2xM5 screw threads
    for( dx=[-ll/2+5,-ll/2+20] )
      translate( [dx,0,0] ) 
        cylinder( d=5.0, h=hh+1, center=true, $fn=50 );

    // 2xM4 screw threads
    for( dx=[ll/2-5,ll/2-20] )
      translate( [dx,0,0] ) 
        cylinder( d=4.0, h=hh+1, center=true, $fn=50 );
  }
     


} // load_cell_TAL220

// aus paper tams hx711 
use <load_cells.scad>
use <nano_scale.scad>