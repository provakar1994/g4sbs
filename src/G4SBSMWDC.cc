#include "G4SBSMWDC.hh"

#include "G4SBSDetectorConstruction.hh"

#include "G4UserLimits.hh"
#include "G4SDManager.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"

#include "sbstypes.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"


G4SBSMWDC::G4SBSMWDC(G4SBSDetectorConstruction *dc):G4SBSComponent(dc){
  fFieldD = 90*um;
  fSignalD = 20*um;
  fWireSep = 1.0*cm;
  fCath2WireDist = 3.0*mm;

  fCathodeThick = 12.0*um;
  fCuThick = 120.0*nm;
  fMylarThick = 12.0*um - 2.0*fCuThick;
  fPlaneThick = 2.0*fCathodeThick + 2.0*fCath2WireDist;

  fGlassThick = 3.0*mm;
  fSpacer = 1.0*mm;

  int chamberN[3] = {  1,   2,   3};
  int nplanes[3]  = {  6,   3,   6};
  int nwires[3]   = {140, 200, 200};

  double wirespace[3] = {1.0*cm, 1.0*cm, 1.0*cm};     // cm
  double height[3]    = {1.40*m, 2.00*m, 2.0*m };     // m
  double width[3]     = {0.35*m, 0.50*m, 0.50*m};     // m
  double z0_dis[3]    = {0.00*m, 0.36*m, 0.71*m};     // m

  G4String pattern[15] = { "U","U","X","X","V","V",
			   "U","X","V",
			   "U","U","X","X","V","V" };
  for(int i=0; i<6; i++){
    fChamber0.push_back(pattern[i]);
  }
  for(int i=6; i<9; i++){
    fChamber1.push_back(pattern[i]);
  }
  for(int i=9; i<15; i++){
    fChamber2.push_back(pattern[i]);
  }

  fGEn_Setup[0] = fChamber0;
  fGEn_Setup[1] = fChamber1;
  fGEn_Setup[2] = fChamber2;
  
  // Fill all the vectors
  for(int i=0; i<3; i++){
    fChamberNumber.push_back( chamberN[i] );
    fNplanes.push_back( nplanes[i] );
    fNwires.push_back( nwires[i] );
    fNwirespacing.push_back( wirespace[i] );
    fNheight.push_back( height[i] );
    fNwidth.push_back( width[i] );
    fDist_z0.push_back( z0_dis[i] );
  }

  mylarVisAtt = new G4VisAttributes(G4Colour( 0.5, 0.5, 0.5 ) );
  mylarVisAtt->SetForceWireframe(true);
  
  cuVisAtt = new G4VisAttributes(G4Colour(0.76,0.47,0.043));
  cuVisAtt->SetForceWireframe(true);

  glassVisAtt = new G4VisAttributes(G4Colour(0.25,0.86,0.94));
  glassVisAtt->SetForceWireframe(true);

  gasVisAtt = new G4VisAttributes(G4Colour(1.0,0.0,0.0,0.08));
  //gasVisAtt->SetForceWireframe(true);

  mothVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  mothVisAtt->SetForceWireframe(true);

  chamVisAtt = new G4VisAttributes(G4Colour(G4Colour::Magenta()));
  chamVisAtt->SetForceWireframe(true);

  sigwireVisAtt = new G4VisAttributes( G4Colour(0.0,0.8,0.0) );
  fieldwireVisAtt = new G4VisAttributes(G4Colour(G4Colour::White()));
 
  // Define rotation for the wires
  fWireRot = new G4RotationMatrix;
  fWireRot->rotateY(90.0*deg);
}

G4SBSMWDC::~G4SBSMWDC(){;}

void G4SBSMWDC::BuildComponent(G4LogicalVolume *){
  ;
}

void G4SBSMWDC::BuildComponent( G4LogicalVolume* world, G4RotationMatrix* rot, 
				G4ThreeVector pos, G4String SDname ) {
  double mX = 1.0*m;
  double mY = 2.1*m;
  double mZ = 1.0*m;
  G4Box* mother = new G4Box("mother", mX/2.0, mY/2.0, mZ/2.0);
  G4LogicalVolume* mother_log = new G4LogicalVolume(mother,GetMaterial("Vacuum"),"mother_log");
  mother_log->SetVisAttributes( G4VisAttributes::Invisible );
  G4ThreeVector origin(0.0,0.0,0.0);

  ///////////////////////////////////////////////////////////////////////////////
  // Cathodes:
  //////////////////////////////////////////////////////////////////////////////

  // Generate the Cathodes for chambers 1-3, and
  // put into a vector

  for(unsigned int i=0; i<fChamberNumber.size(); i++){
    G4Box* cathode_moth = new G4Box("temp", fNwidth[i]/2.0, fNheight[i]/2.0, fCathodeThick/2.0);
    G4Box* copper_box = new G4Box("copper_box", fNwidth[i]/2.0, fNheight[i]/2.0, fCuThick/2.0);
    G4Box* mylar_box = new G4Box("mylar_box", fNwidth[i]/2.0, fNheight[i]/2.0, fMylarThick/2.0);

    G4LogicalVolume* cathode_log = new G4LogicalVolume(cathode_moth, GetMaterial("Air"), "cathode_log");
    G4LogicalVolume* copper_log = new G4LogicalVolume(copper_box, GetMaterial("Copper"), "copper_log");
    G4LogicalVolume* mylar_log = new G4LogicalVolume(mylar_box, GetMaterial("Mylar"), "mylar_log");

    // Colors:
    cathode_log->SetVisAttributes( G4VisAttributes::Invisible );
    copper_log->SetVisAttributes( cuVisAtt );
    mylar_log->SetVisAttributes( mylarVisAtt );

    double z = fCathodeThick/2.0 - fCuThick/2.0;
    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, z), copper_log, "copper_phys_back", cathode_log, false, 0);
    z = -fCathodeThick/2.0 + fCuThick/2.0;
    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, z), copper_log, "copper_phys_front", cathode_log, false, 1);
    new G4PVPlacement(0, origin, mylar_log, "mylar_phys", cathode_log, false, 0);
    
    fCathodes[i] = cathode_log;
  }
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // CHAMBERS
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  std::map<int,std::vector<G4String> >::iterator mit;
  std::vector<G4String>::iterator vit;
  char temp_name[255];
  int copyID = 0;
  double mother_length = 0.0;
  for( mit = fGEn_Setup.begin(); mit != fGEn_Setup.end(); mit++ ) {
    // Make a Chamber:
    int chamber_number = mit->first;
    int num_planes = (mit->second).size();
    double chamber_thick = fPlaneThick*num_planes + 2.0*fGlassThick + fSpacer*(num_planes+1);
    mother_length+=chamber_thick;
    sprintf(temp_name, "chamber_%1d_box", chamber_number);
    G4Box *chamber_temp = new G4Box(temp_name, fNwidth[chamber_number]/2.0, 
    				    fNheight[chamber_number]/2.0, chamber_thick/2.0);
  
    sprintf(temp_name, "chamber_%1d_log", chamber_number);
    G4LogicalVolume* chamber_log = new G4LogicalVolume(chamber_temp, GetMaterial("Air"), temp_name);
    chamber_log->SetVisAttributes( chamVisAtt );
    
    // Make the glass, and place it at front / back of a chamber:    
    G4Box* glass_winbox = new G4Box("glass_winbox", fNwidth[chamber_number]/2.0, 
    				    fNheight[chamber_number]/2.0, fGlassThick/2.0);
    G4LogicalVolume* glass_winlog = new G4LogicalVolume(glass_winbox, GetMaterial("Glass"), 
    							"glass_winlog" );
    glass_winlog->SetVisAttributes( glassVisAtt );

    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, -chamber_thick/2.0 + fGlassThick/2.0),
    		      glass_winlog, "glass_phys_front", chamber_log, false, 0, true);
    new G4PVPlacement(0, G4ThreeVector(0.0, 0.0,  chamber_thick/2.0 - fGlassThick/2.0),
    		      glass_winlog, "glass_phys_back", chamber_log, false, 0 , true);
    
    int planeN = 0;

    // Fill the chamber with planes
    for( vit = mit->second.begin(); vit != mit->second.end(); vit++ ) {

      G4String plane_type = *vit;
      G4LogicalVolume* plane_log;
   
      if( plane_type == "X" ) {
      	plane_log = BuildX( fNwidth[chamber_number], fNheight[chamber_number], chamber_number, planeN, copyID);
      } else {
	plane_log = BuildUorV(fNwidth[chamber_number], fNheight[chamber_number], plane_type, chamber_number, planeN, copyID);
      }
 
      double z = -chamber_thick/2.0 + fGlassThick + (planeN+0.5)*fPlaneThick + fSpacer*(planeN+1);
      sprintf(temp_name, "chamber%1d_plane%1d_log", chamber_number, planeN);
      new G4PVPlacement(0, G4ThreeVector(0.0,0.0,z), plane_log, temp_name, chamber_log, false, copyID,true);

      planeN++;
      copyID++;
    }
    sprintf(temp_name, "chamber%1d_phys",chamber_number);    
    new G4PVPlacement(0,G4ThreeVector(0.0,0.0, -mZ/2.0 + chamber_thick/2.0 + fDist_z0[chamber_number]), 
    		      chamber_log, temp_name, mother_log, false, chamber_number,true);
  }
  new G4PVPlacement(rot, pos, mother_log, "MWDC_mother_phys", world, false, 0);
}


// Arguments: width = plane width
//            height = plane height
//            chamber = chamber #
//            planeN = plane number
//            offindex = counting device to get signal/field wire offset. Successive planes
//                       have signal wires offset by 0.5*cm in order to increase the resolution
G4LogicalVolume* G4SBSMWDC::BuildX(double width, double height, int chamber, int planeN, int offindex) {
  char temp_name[255];
  sprintf(temp_name, "X_chamber%1d_plane%1d_box", chamber, planeN);
 
  G4Box* Xbox = new G4Box(temp_name, width/2.0, height/2.0, fPlaneThick/2.0);

  sprintf(temp_name, "X_chamber%1d_plane%1d_log", chamber, planeN);
  G4LogicalVolume* Xlog = new G4LogicalVolume( Xbox, GetMaterial("MWDC_gas"), temp_name );
  Xlog->SetVisAttributes( gasVisAtt );

  // Put in the cathodes
  sprintf(temp_name, "X_chamber%1d_plane%1d_cathodefront", chamber, planeN);
  new G4PVPlacement(0, G4ThreeVector(0.0,0.0,-fPlaneThick/2.0 + fCathodeThick/2.0), fCathodes[chamber], 
  		    temp_name, Xlog, false, 0, true);
  sprintf(temp_name, "X_chamber%1d_plane%1d_cathodeback", chamber, planeN);
  new G4PVPlacement(0, G4ThreeVector(0.0,0.0,fPlaneThick/2.0 - fCathodeThick/2.0), fCathodes[chamber], 
  		    temp_name, Xlog, false, 0, true);

  // Now Let's make the signal / field wires:
  G4Tubs *signaltub = new G4Tubs( "wire_tub", 0.0*cm, fSignalD/2.0, width/2.0, 0.0, twopi );
  G4Tubs *fieldtub = new G4Tubs( "field_tub", 0.0*cm, fFieldD/2.0, width/2.0, 0.0, twopi );

  // Signal wire is actually gold plated-tungsten, not sure on the dimensions so I simply chose Tungsten
  G4LogicalVolume* signal_log = new G4LogicalVolume( signaltub, GetMaterial("Tungsten"), "signal_log" );
  signal_log->SetVisAttributes( sigwireVisAtt );

  // Field wire is actually copper-beryllium, but beryllium (according to google) is < 3% so I ignored it
  G4LogicalVolume* field_log = new G4LogicalVolume( fieldtub, GetMaterial("Copper"), "field_log" );
  field_log->SetVisAttributes( fieldwireVisAtt );

  // Lets Generate the wire mesh:
  double offset = 0.0*cm;
  if( (offindex+1) % 2 == 0 ) {
    offset = fWireSep / 2.0; // should be 0.5*cm for GEn
  }

  int field_count = fNwires[chamber];

  for( int i=0; i<fNwires[chamber]; i++ ){
    double y_signal = -height/2.0 + i*fWireSep + offset;
    double y_field  = y_signal + fWireSep/2.0;

    sprintf(temp_name, "X_chamber%1d_plane%1d_signalwire%1d", chamber, planeN, i);
    new G4PVPlacement( fWireRot, G4ThreeVector(0.0, y_signal, 0.0), signal_log, temp_name,
		       Xlog, false, i, true);

    sprintf(temp_name, "X_chamber%1d_plane%1d_fieldwire%1d", chamber, planeN, field_count); 
    new G4PVPlacement( fWireRot, G4ThreeVector(0.0, y_field, 0.0), field_log, temp_name,
		       Xlog, false, field_count, true );

    field_count++;
  }

 return Xlog;
}

G4LogicalVolume* G4SBSMWDC::BuildUorV(double width, double height, G4String type, int chamber, int planeN, int offindex) {
  char temp_name[255];
  char temp_namelog[255];
  if( type == "U" ) {
    sprintf(temp_name, "U_chamber%1d_plane%1d_box", chamber, planeN);
    sprintf(temp_namelog, "U_chamber%1d_plane%1d_log", chamber, planeN);
  } else {
    sprintf(temp_name, "V_chamber%1d_plane%1d_box", chamber, planeN);
    sprintf(temp_namelog, "V_chamber%1d_plane%1d_log", chamber, planeN);
  }
 
  G4Box* UorVbox = new G4Box(temp_name, width/2.0, height/2.0, fPlaneThick/2.0);

  G4LogicalVolume* UorVlog = new G4LogicalVolume( UorVbox, GetMaterial("MWDC_gas"), temp_namelog );
  UorVlog->SetVisAttributes( gasVisAtt );

  // Put in the cathodes
  if( type == "U" ) {
  sprintf(temp_name, "U_chamber%1d_plane%1d_cathodefront", chamber, planeN);
  sprintf(temp_name, "U_chamber%1d_plane%1d_cathodeback", chamber, planeN);
  } else {
  sprintf(temp_name, "V_chamber%1d_plane%1d_cathodefront", chamber, planeN);
  sprintf(temp_name, "V_chamber%1d_plane%1d_cathodeback", chamber, planeN);
  }

  new G4PVPlacement(0, G4ThreeVector(0.0,0.0,-fPlaneThick/2.0 + fCathodeThick/2.0), fCathodes[chamber], 
  		    temp_name, UorVlog, false, 0 );

  new G4PVPlacement(0, G4ThreeVector(0.0,0.0,fPlaneThick/2.0 - fCathodeThick/2.0), fCathodes[chamber], 
  		    temp_name, UorVlog, false, 0 );

  return UorVlog;
}