/*
  This file is part of Fec Software project.
  
  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#include <iostream>

#include "deviceFrame.h"
#include "FecAccessManager.h"

/** Inialising the attributs of the class
 */
FecAccessManager::FecAccessManager ( FecAccess *fecAccess, bool displayDebugMessage ) {

  lastOperationNumberErrors_ = 0 ;
  haltStateMachine_ = false ;
  maxErrorAllowed_ = 0 ; // wait for ever on the errors

  fecAccess_ = fecAccess ;

  apvSet_.clear ( ) ;
  pllSet_.clear ( ) ;
  laserdriverSet_.clear ( ) ;
  dohSet_.clear ( ) ;
  muxSet_.clear ( ) ;
  philipsSet_.clear ( ) ;
  dcuSet_.clear ( ) ;
  piaSet_.clear ( ) ;
#ifdef PRESHOWER
  deltaSet_.clear ( ) ;
  paceSet_.clear ( ) ;
  kchipSet_.clear ( ) ;
  gohSet_.clear ( ) ; 
#endif // PRESHOWER
#ifdef TOTEM
  vfatSet_.clear ( ) ;
  cchipSet_.clear ( ) ;
  tbbSet_.clear ( ) ;
#endif // TOTEM
  displayDebugMessage_ = displayDebugMessage ;
}

/** Destroy each access store in the different hash tables and destroy all the
 * map created
 */
FecAccessManager::~FecAccessManager ( ) {

  removeDevices ( ) ;
}

/** Destroy each access store in the different hash tables for all the device
 * that was initialised. 
 */
void FecAccessManager::removeDevices ( ) {

  removeDevices (APV25) ;
  removeDevices (PLL) ;
  removeDevices (LASERDRIVER) ;
  removeDevices (DOH) ;
  removeDevices (APVMUX) ;
  removeDevices (PHILIPS) ;
  removeDevices (DCU) ;
  removeDevices (PIARESET) ;
#ifdef PRESHOWER
  removeDevices (DELTA) ;
  removeDevices (PACE) ;
  removeDevices (KCHIP) ;
  removeDevices (GOH) ;
#endif // PRESHOWER
#ifdef TOTEM
  removeDevices (VFAT) ;
  removeDevices (CCHIP) ;
  removeDevices (TBB) ;
#endif // TOTEM
}

/** Destroy each access store in the different hash tables for all the device
 * that was initialised. 
 * \param type - device type to be removed
 */
void FecAccessManager::removeDevices ( enumDeviceType type ) {

  // Destroy all the device connected
  if (type == APV25) {
    for (apvAccessedType::iterator p=apvSet_.begin();p!=apvSet_.end();p++) {

      // Retreive the device connected
      apvAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in FecAccessManager::removeDevices (APV25)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear APV table
    apvSet_.clear  ( )        ;

    return ;
  }

  // Destroy all the device connected
  if (type == PLL) {
    for (pllAccessedType::iterator p=pllSet_.begin();p!=pllSet_.end();p++) {
      
      // Retreive the device connected
      pllAccess *device = p->second ;
      
      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in FecAccessManager::removeDevices (PLL)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear PLL table
    pllSet_.clear ( ) ;

    return ;
  }

  // Destroy all the device connected
  if (type == LASERDRIVER) {
    for (laserdriverAccessedType::iterator p=laserdriverSet_.begin();p!=laserdriverSet_.end();p++) {
      
      // Retreive the device connected
      laserdriverAccess *device = p->second ;
      
      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in FecAccessManager::removeDevices (LASERDRIVER)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear LASERDRIVER table
    laserdriverSet_.clear ( ) ;

    return ;
  }

  // Destroy all the device connected
  if (type == DOH) {
    for (dohAccessedType::iterator p=dohSet_.begin();p!=dohSet_.end();p++) {
      
      // Retreive the device connected
      DohAccess *device = p->second ;
      
      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in FecAccessManager::removeDevices (DOH)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear DOH table
    dohSet_.clear ( ) ;

    return ;
  }

  // Destroy all the device connected
  if (type == APVMUX) {
    for (muxAccessedType::iterator p=muxSet_.begin();p!=muxSet_.end();p++) {

      // Retreive the device connected
      muxAccess *device = p->second ;
      
      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in FecAccessManager::removeDevices (MUX)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear APVMUX table
    muxSet_.clear  ( )        ;

    return ;
  }

  // Destroy all the device connected
  if (type == PHILIPS) {
    for (philipsAccessedType::iterator p=philipsSet_.begin();p!=philipsSet_.end();p++) {

      // Retreive the device connected
      philipsAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;        

        std::cout << "Problem in FecAccessManager::removeDevices (PHILIPS)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear PHILIPS table
    philipsSet_.clear ( )     ;

    return ;
  }

  // Destroy all the device connected
  if (type == DCU) {
    for (dcuAccessedType::iterator p=dcuSet_.begin();p!=dcuSet_.end();p++) {
  
      // Retreive the device connected
      dcuAccess *device = p->second ;
      
      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;        

        std::cout << "Problem in FecAccessManager::removeDevices (DCU) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear DCU table
    dcuSet_.clear  ( )        ;

    return ;
  }

  if (type == PIARESET) {

    // Destroy all the device connected
    for (piaAccessedType::iterator p=piaSet_.begin();p!=piaSet_.end();p++) {

      // Retreive the device connected
      PiaResetAccess *pia = p->second ;

      try {
        // Delete it
        delete pia ;
      }
      catch (FecExceptionHandler &e) {
        
        char msg[80] ;
        decodeKey (msg, pia->getKey()) ;

        std::cout << "Problem in FecAccessManager::removeDevices (PIA) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear PIA table
    piaSet_.clear  ( )        ;

    return ;
  }

#ifdef PRESHOWER
  // Destroy all the device connected
  if (type == DELTA) {
    for (deltaAccessedType::iterator p=deltaSet_.begin();p!=deltaSet_.end();p++) {

      // Retreive the device connected
      deltaAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in FecAccessManager::removeDevices (DELTA)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear DELTA table
    deltaSet_.clear  ( )        ;

    return ;
  }

// Destroy all the device connected
  if (type == PACE) {
    for (paceAccessedType::iterator p=paceSet_.begin();p!=paceSet_.end();p++) {

      // Retreive the device connected
      paceAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in FecAccessManager::removeDevices (PACE)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear PACE table
    paceSet_.clear  ( )        ;

    return ;
  }

// Destroy all the device connected
  if (type == KCHIP) {
    for (kchipAccessedType::iterator p=kchipSet_.begin();p!=kchipSet_.end();p++) {

      // Retreive the device connected
      kchipAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in FecAccessManager::removeDevices (KCHIP)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear KCHIP table
    kchipSet_.clear  ( )        ;
    return ;
  }
// Destroy all the device connected
  if (type == GOH) {
    for (gohAccessedType::iterator p=gohSet_.begin();p!=gohSet_.end();p++) {

      // Retreive the device connected
      gohAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in FecAccessManager::removeDevices (GOH)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear KCHIP table
    gohSet_.clear  ( ) ;
    return ;
  }

#endif // PRESHOWER

#ifdef TOTEM
  // Destroy all the device connected
  if (type == VFAT) {
    for (vfatAccessedType::iterator p=vfatSet_.begin();p!=vfatSet_.end();p++) {

      // Retreive the device connected
      vfatAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in FecAccessManager::removeDevices (VFAT)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear VFAT table
    vfatSet_.clear  ( )        ;

    return ;
  }

  // Destroy all the device connected
  if (type == CCHIP) {
    for (cchipAccessedType::iterator p=cchipSet_.begin();p!=cchipSet_.end();p++) {

      // Retreive the device connected
      totemCChipAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in FecAccessManager::removeDevices (CCHIP)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear CCHIP table
    cchipSet_.clear  ( )        ;

    return ;
  }

  // Destroy all the device connected
  if (type == TBB) {
    for (tbbAccessedType::iterator p=tbbSet_.begin();p!=tbbSet_.end();p++) {

      // Retreive the device connected
      totemBBAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in FecAccessManager::removeDevices (TBB)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear CCHIP table
    tbbSet_.clear  ( )        ;

    return ;
  }
#endif // TOTEM
}

/** 
 * \param displayDebugMessage - display the debug message
 */
void FecAccessManager::setDisplayDebugMessage ( bool displayDebugMessage ) {

  displayDebugMessage_ = displayDebugMessage ;
}

/**
 * \return boolean to know if display the debug message is on or off
 */
bool FecAccessManager::getDisplayDebugMessage ( ) {

  return displayDebugMessage_ ;
}

/**
 * \return return the FEC access object
 * \warning do not delete it
 */
FecAccess *FecAccessManager::getFecAccess ( ) {

  return fecAccess_ ;
}

/** 
 * \param deviceType - device type
 * \param index - key of the device connected
 * \return access of the device (it is the real access and not a copy, do not delete it !)
 * \exception FecExceptionHandler
 * \warning Only access that inherits from deviceAccess class can be used in this methods
 */
deviceAccess *FecAccessManager::getAccess ( enumDeviceType deviceType, keyType index ) {
 
  deviceAccess *device = NULL ;

  if (deviceType == APV25) {
    // Try to find the Apv in the map
    apvAccessedType::iterator apvmap = apvSet_.find(index) ;
    if (apvmap == apvSet_.end()) device = NULL ;
    else device = apvmap->second ;
  } 
  
  else if (deviceType == PLL) {
    // Try to find the Pll in the map
    pllAccessedType::iterator pllmap = pllSet_.find(index) ;
    if (pllmap == pllSet_.end()) device = NULL ;
    else device = pllmap->second ;
  } 

  else if (deviceType == LASERDRIVER) {
    // Try to find the Laserdriver in the map
    laserdriverAccessedType::iterator laserdrivermap = laserdriverSet_.find(index) ;
    if (laserdrivermap == laserdriverSet_.end()) device = NULL ;
    else device = laserdrivermap->second ;
  } 

  else if (deviceType == DOH) {
    // Try to find the Doh in the map
    dohAccessedType::iterator dohmap = dohSet_.find(index) ;
    if (dohmap == dohSet_.end()) device = NULL ;
    else device = dohmap->second ;
  } 
  
  else if (deviceType == APVMUX) {
    // Try to find the Mux in the map
    muxAccessedType::iterator muxmap = muxSet_.find(index) ;
    if (muxmap == muxSet_.end()) device = NULL ;
    else device = muxmap->second ;
  } 

  else if (deviceType == DCU) {
    // Try to find the Dcu in the map
    dcuAccessedType::iterator dcumap = dcuSet_.find(index) ;
    if (dcumap == dcuSet_.end()) device = NULL ;
    else device = dcumap->second ;

  } 

  else if (deviceType == PHILIPS) {
    // Try to find the Philips in the map
    philipsAccessedType::iterator philipsmap = philipsSet_.find(index) ;
    if (philipsmap == philipsSet_.end()) device = NULL ;
    else device = philipsmap->second ;
  }

#ifdef PRESHOWER
  else if (deviceType == DELTA) {
    // Try to find the Delta in the map
    deltaAccessedType::iterator deltamap = deltaSet_.find(index) ;
    if (deltamap == deltaSet_.end()) device = NULL ;
    else device = deltamap->second ;
  }

  else if (deviceType == PACE) {
    // Try to find the PaceAM in the map
    paceAccessedType::iterator pacemap = paceSet_.find(index) ;
    if (pacemap == paceSet_.end()) device = NULL ;
    else device = pacemap->second ;
  }

  else if (deviceType == KCHIP) {
    // Try to find the Kchip in the map
    kchipAccessedType::iterator kchipmap = kchipSet_.find(index) ;
    if (kchipmap == kchipSet_.end()) device = NULL ;
    else device = kchipmap->second ;
  }

  else if (deviceType == GOH) {
    // Try to find the Kchip in the map
    gohAccessedType::iterator gohmap = gohSet_.find(index) ;
    if (gohmap == gohSet_.end()) device = NULL ;
    else device = gohmap->second ;
  }
#endif // PRESHOWER

#ifdef TOTEM
  else if (deviceType == VFAT) {
    // Try to find the Vfat in the map
    vfatAccessedType::iterator vfatmap = vfatSet_.find(index) ;
    if (vfatmap == vfatSet_.end()) device = NULL ;
    else device = vfatmap->second ;
  }

  else if (deviceType == CCHIP) {
    // Try to find the CChip in the map
    cchipAccessedType::iterator cchipmap = cchipSet_.find(index) ;
    if (cchipmap == cchipSet_.end()) device = NULL ;
    else device = cchipmap->second ;
  }

  else if (deviceType == TBB) {
    // Try to find the Tbb in the map
    tbbAccessedType::iterator tbbmap = tbbSet_.find(index) ;
    if (tbbmap == tbbSet_.end()) device = NULL ;
    else device = tbbmap->second ;
  }
#endif // TOTEM

  // Note that the PIA access cannot be returned in this method
  return (device) ;
}

/** 
 * \param index - key of the device connected
 * \return PIA access (it is the real access and not a copy, do not delete it !)
 * \exception FecExceptionHandler
 * \warning Only PIA access can be used in this methods
 */
PiaResetAccess *FecAccessManager::getPiaAccess ( keyType index ) {

  PiaResetAccess *piaAccess = NULL ;

  // Try to find the PIA access in the map
  piaAccessedType::iterator piamap = piaSet_.find(index) ;
  if (piamap != piaSet_.end()) piaAccess = piamap->second ;

  return piaAccess ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
dcuAccessedType &FecAccessManager::getDcuAccesses ( ) {
 
  return (dcuSet_) ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
pllAccessedType &FecAccessManager::getPllAccesses ( ) {
 
  return (pllSet_) ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
apvAccessedType &FecAccessManager::getApvAccesses ( ) {
 
  return (apvSet_) ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
laserdriverAccessedType &FecAccessManager::getLaserdriverAccesses ( ) {
 
  return (laserdriverSet_) ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
laserdriverAccessedType &FecAccessManager::getAOHAccesses ( ) {
 
  return (laserdriverSet_) ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
dohAccessedType &FecAccessManager::getDOHAccesses ( ) {
 
  return (dohSet_) ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
muxAccessedType &FecAccessManager::getApvMuxAccesses ( ) {
 
  return (muxSet_) ;
}

/** 
 * \return all the accesses from the map pllAccessedType (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
philipsAccessedType &FecAccessManager::getPhilipsAccesses ( ) {
 
  return (philipsSet_) ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
piaAccessedType &FecAccessManager::getPiaResetAccesses ( ) {
 
  return (piaSet_) ;
}

#ifdef PRESHOWER

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
deltaAccessedType &FecAccessManager::getDeltaAccesses ( ) {
 
  return (deltaSet_) ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
paceAccessedType &FecAccessManager::getPaceAccesses ( ) {
 
  return (paceSet_) ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
kchipAccessedType &FecAccessManager::getKchipAccesses ( ) {
 
  return (kchipSet_) ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
gohAccessedType &FecAccessManager::getGohAccesses ( ) {
 
  return (gohSet_) ;
}

#endif // PRESHOWER

#ifdef TOTEM

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
vfatAccessedType &FecAccessManager::getVfatAccesses ( ) {
 
  return (vfatSet_) ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
cchipAccessedType &FecAccessManager::getCchipAccesses ( ) {
 
  return (cchipSet_) ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
tbbAccessedType &FecAccessManager::getTbbAccesses ( ) {
 
  return (tbbSet_) ;
}

#endif // TOTEM

/**
 * \param access - device access class
 * \exception FecExceptionHandler
 */
void FecAccessManager::setAccess ( deviceAccess *access ) throw (FecExceptionHandler) {

  keyType index = access->getKey() ;
  enumDeviceType deviceType = access->getDeviceType() ;

  if (deviceType == APV25) {
    // Try to find the Apv in the map
    apvAccessedType::iterator apvmap = apvSet_.find(index) ;
    if (apvmap == apvSet_.end()) {
      
      // Add the access to the map
      apvSet_[index] = (apvAccess *)access ;
    }
    else {
      
      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (APV) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 
  
  else if (deviceType == PLL) {
    // Try to find the Pll in the map
    pllAccessedType::iterator pllmap = pllSet_.find(index) ;
    if (pllmap == pllSet_.end()) {
      
      // Add the access to the map
      pllSet_[index] = (pllAccess *)access ;
    }
    else {
      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (PLL) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 

  else if (deviceType == LASERDRIVER) {
    // Try to find the Laserdriver in the map
    laserdriverAccessedType::iterator laserdrivermap = laserdriverSet_.find(index) ;
    if (laserdrivermap == laserdriverSet_.end()) {

      // Add the access to the map
      laserdriverSet_[index] =  (laserdriverAccess *)access ;
    }
    else {
      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (AOH) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 

  else if (deviceType == DOH) {
    // Try to find the Doh in the map
    dohAccessedType::iterator dohmap = dohSet_.find(index) ;
    if (dohmap == dohSet_.end()) {

      // Add the access to the map
      dohSet_[index] =  (DohAccess *)access ;
    }
    else {
     RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					     "device (DOH) already connected",
					     ERRORCODE,
					     index) ;
    }
  } 

  else if (deviceType == APVMUX) {
    // Try to find the Mux in the map
    muxAccessedType::iterator muxmap = muxSet_.find(index) ;
    if (muxmap == muxSet_.end()) {

      // Add the access to the map
      muxSet_[index] = (muxAccess *)access ;
    }
    else {
      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (APV MUX) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 
  
  else if (deviceType == DCU) {
    // Try to find the Dcu in the map
    dcuAccessedType::iterator dcumap = dcuSet_.find(index) ;
    if (dcumap == dcuSet_.end()) {

      // Add the access to the map
      dcuSet_[index] = (dcuAccess *)access ;
    }
    else {
     RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					     "device (DCU) already connected",
					     ERRORCODE,
					     index) ;
    }
  } 

  else if (deviceType == PHILIPS) {
    // Try to find the Philips in the map
    philipsAccessedType::iterator philipsmap = philipsSet_.find(index) ;
    if (philipsmap == philipsSet_.end()) {
      
      // Add the access to the map
      philipsSet_[index] = (philipsAccess *)access ;
    }
    else {
     RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				  "device (Philips) already connected",
				  ERRORCODE,
				  index) ;
    }
  } 

#ifdef PRESHOWER
  else if (deviceType == DELTA) {
    // Try to find the Delta in the map
    deltaAccessedType::iterator deltamap = deltaSet_.find(index) ;
    if (deltamap == deltaSet_.end()) {
      
      // Add the access to the map
      deltaSet_[index] = (deltaAccess *)access ;
    }
    else {
      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (DELTA) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 

  else if (deviceType == PACE) {
    // Try to find the PaceAM in the map
    paceAccessedType::iterator pacemap = paceSet_.find(index) ;
    if (pacemap == paceSet_.end()) {
      
      // Add the access to the map
      paceSet_[index] = (paceAccess *)access ;
    }
    else {
     RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				  "device (PACE) already connected",
				  ERRORCODE,
				  index) ;
    }
  } 

  else if (deviceType == KCHIP) {
    // Try to find the Kchip in the map
    kchipAccessedType::iterator kchipmap = kchipSet_.find(index) ;
    if (kchipmap == kchipSet_.end()) {
      
      // Add the access to the map
      kchipSet_[index] = (kchipAccess *)access ;
    }
    else {
     RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				  "device (KCHIP) already connected",
				  ERRORCODE,
				  index) ;
    }
  } 

  else if (deviceType == GOH) {
    // Try to find the GOH in the map
    gohAccessedType::iterator gohmap = gohSet_.find(index) ;
    if (gohmap == gohSet_.end()) {
      
      // Add the access to the map
      gohSet_[index] = (gohAccess *)access ;
    }
    else {
     RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				  "device (GOH) already connected",
				  ERRORCODE,
				  index) ;
    }
  } 
#endif // PRESHOWER

#ifdef TOTEM
  else if (deviceType == VFAT) {
    // Try to find the Vfat in the map
    vfatAccessedType::iterator vfatmap = vfatSet_.find(index) ;
    if (vfatmap == vfatSet_.end()) {
      
      // Add the access to the map
      vfatSet_[index] = (vfatAccess *)access ;
    }
    else {
     RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				  "device (VFAT) already connected",
				  ERRORCODE,
				  index) ;
    }
  }

  else if (deviceType == CCHIP) {
    // Try to find the Cchip in the map
    cchipAccessedType::iterator cchipmap = cchipSet_.find(index) ;
    if (cchipmap == cchipSet_.end()) {
      
      // Add the access to the map
      cchipSet_[index] = (totemCChipAccess *)access ;
    }
    else {
     RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				  "device (CCHIP) already connected",
				  ERRORCODE,
				  index) ;
    }
  }

  else if (deviceType == TBB) {
    // Try to find the Tbb in the map
    tbbAccessedType::iterator tbbmap = tbbSet_.find(index) ;
    if (tbbmap == tbbSet_.end()) {
      
      // Add the access to the map
      tbbSet_[index] = (totemBBAccess *)access ;
    }
    else {
     RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				  "device (TBB) already connected",
				  ERRORCODE,
				  index) ;
    }
  }
#endif // TOTEM

  else {
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( XDAQFEC_INVALIDOPERATION,
				       "device type unknown",
				       ERRORCODE,
				       index,
				       "device type", deviceType) ;
  }
}

/**
 * \param vDevice - a vector of devices (description)
 * \param errorList - list of exceptions during the download
 * \param pllReset - force the PLL reset at the starting (default false)
 * \param dohSet - set the DOH (default true)
 * \return error number
 * \exception FecHandlerException
 */
unsigned int FecAccessManager::downloadValues ( deviceVector *vDevice, std::list<FecExceptionHandler *> &errorList, bool pllReset, bool dohSet ) throw (FecExceptionHandler) {

  if (displayDebugMessage_) 
    std::cout << __PRETTY_FUNCTION__ << ": " << std::dec << vDevice->size() << " devices to be downloaded" << std::endl ;

  // A message
  char msg[MAXCHARDECODEKEY] ;

  // Number of errors
  unsigned int error = 0 ;
  
  if ( (vDevice != NULL) && (!vDevice->empty()) ) {

    // For each device => access it
    for (deviceVector::iterator device = vDevice->begin() ; (device != vDevice->end()) && (! haltStateMachine_) && ( (error < maxErrorAllowed_) || (maxErrorAllowed_ == 0) ); device ++) {

      deviceDescription *deviced = *device ;

      switch (deviced->getDeviceType()) {
      case APV25: {
	apvDescription *apvDevice = (apvDescription *)deviced ;
	error += parseApv ( *apvDevice, errorList ) ;
	break ;
      }
      case PLL: {
	pllDescription *pllDevice = (pllDescription *)deviced ;
	error += parsePll ( *pllDevice, errorList, true, pllReset ) ;
	break ;
      }
      case DOH:
      case LASERDRIVER: {
	laserdriverDescription *laserdriverDevice = (laserdriverDescription *)deviced ;
	// Address 0x60 for laserdriver
	// Address DOHI2CADDRESS for DOH (defined in DohAccess.h)
	if (laserdriverDevice->getAddress() == DOHI2CADDRESS)
	  error += parseDoh ( *laserdriverDevice, errorList, dohSet ) ;
	else
	  error += parseLaserdriver ( *laserdriverDevice, errorList ) ;
	break ;
      }
      case APVMUX: {
	muxDescription *muxDevice = (muxDescription *)deviced ;
	error += parseMux ( *muxDevice, errorList ) ;
	break ;
      }
      case PHILIPS: {
	philipsDescription *philipsDevice = (philipsDescription *)deviced ;
	error += parsePhilips ( *philipsDevice, errorList ) ;
	break ;
      }
      case DCU: {
	dcuDescription *dcuDevice = (dcuDescription *)deviced ;
	error += parseDcu ( *dcuDevice, errorList ) ;
	break ;
      }
#ifdef PRESHOWER
      case DELTA: {
	deltaDescription *deltaDevice = (deltaDescription *)deviced ;
	error += parseDelta ( *deltaDevice, errorList ) ;
	break ;
      }
      case PACE: {
	paceDescription *paceDevice = (paceDescription *)deviced ;
	error += parsePace ( *paceDevice, errorList ) ;
	break ;
      }
      case KCHIP: {
	kchipDescription *kchipDevice = (kchipDescription *)deviced ;
	error += parseKchip ( *kchipDevice, errorList ) ;
	break ;
      }
      case GOH: {
	gohDescription *gohDevice = (gohDescription *)deviced ;
	error += parseGoh ( *gohDevice, errorList ) ;
	break ;
      }
#endif // PRESHOWER
	  
#ifdef TOTEM
      case VFAT: {
	vfatDescription *vfatDevice = (vfatDescription *)deviced ;
	error += parseVfat ( *vfatDevice, errorList ) ;
	break ;
      }

      case CCHIP: {
	totemCChipDescription *cchipDevice = (totemCChipDescription *)deviced ;
	error += parseCChip ( *cchipDevice, errorList ) ;
	break ;
      }

      case TBB: {
	totemBBDescription *tbbDevice = (totemBBDescription *)deviced ;
	error += parseTbb ( *tbbDevice, errorList ) ;
	break ;
      }
#endif // TOTEM
	  
      default:
	decodeKey (msg, deviced->getKey()) ;
	std::cerr << "Unknown device: 0x" << std::hex << deviced->getDeviceType() << " (" << msg << ")" << std::endl ;
      }
    }
  }
  else {

    RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE,
			       "no device found to be downloaded",
			       ERRORCODE) ;
  }

  // Adding an error if we raised the maximum number of errors
  if ( (maxErrorAllowed_ > 0) && (error >= maxErrorAllowed_) ) {
    FecExceptionHandler *e = NEWFECEXCEPTIONHANDLER ( TOOMUCHERROR,
						      "Maximum number of errors (" + toString(maxErrorAllowed_) + ") reached",
						      CRITICALERRORCODE) ;
    errorList.push_back(e) ;
  }

  haltStateMachine_ = false ;

  lastOperationNumberErrors_ = error ;

  return (error) ;
}


/** calibrate the DOH (find for a specific gain the bias values)
 * \param dohDevice - device to be calibrated (can be empty to calibration all DOH in the table)
 * \param errorList - list of errors
 * \param gain - gain value for each of the laserdriver (1 by default)
 * \warning the calibration of the DOH needs FEC ring reset so all the devices are removed at the end of the calibration and this method generates warning on the console
 */
unsigned int FecAccessManager::calibrateDOH ( deviceVector &dohDevices, std::list<FecExceptionHandler *> &errorList, unsigned char gain ) {
  
  if (displayDebugMessage_) 
    std::cout << std::dec << dohDevices.size() << " DOH to be calibrated" << std::endl ;

  // Remove all the devices to avoid problem with the next delete
  removeDevices() ;

  // Number of errors
  unsigned int error = 0 ;
  
  if (!dohDevices.empty()) {
    // For each device => access it
    for (deviceVector::iterator device = dohDevices.begin() ; (device != dohDevices.end()) && (! haltStateMachine_) &&  ( (error < maxErrorAllowed_) || (maxErrorAllowed_ == 0) ); device ++) {

      deviceDescription *deviced = *device ;

      switch (deviced->getDeviceType()) {
      case LASERDRIVER:
      case DOH: {
	laserdriverDescription *laserdriverDevice = (laserdriverDescription *)deviced ;
	if (displayDebugMessage_) {
	  char msg[80] ;
	  decodeKey(msg,laserdriverDevice->getKey()) ;
	  std::cout << "DOH on " << msg << std::endl ;
	}
	if (laserdriverDevice->getAddress() == DOHI2CADDRESS) {
	  // Do not set it
	  unsigned err = parseDoh ( *laserdriverDevice, errorList, false ) ;
	  if (!err) {
	    try {
	      laserdriverDescription *dohd = dohSet_[deviced->getKey()]->calibrateDOH( displayDebugMessage_, gain /*(min bias value can be set here)*/  ) ;
	      laserdriverDevice->setGain ( dohd->getGain() ) ;
	      tscType8 bias[MAXLASERDRIVERCHANNELS] ;
	      dohd->getBias ( bias ) ;
	      laserdriverDevice->setBias ( bias ) ;
	      delete dohd ;
	    }
	    catch (FecExceptionHandler &e) {
	      errorList.push_back(e.clone()) ;
	    }
	  }
	  else error += err ;

	  break ;
	}
      }
      default: {
	if (displayDebugMessage_) 
	  std::cerr << "Only DOH devices should be set for DOH calibration" << std::endl;
      }
      }
    }
  }

  // Remove all the devices to avoid problem with the next delete
  removeDevices() ;

  // Adding an error if we raised the maximum number of errors
  if ( (maxErrorAllowed_ > 0) && (error >= maxErrorAllowed_) ) {
    FecExceptionHandler *e = NEWFECEXCEPTIONHANDLER ( TOOMUCHERROR,
						      "Maximum number of errors (" + toString(maxErrorAllowed_) + ") reached",
						      CRITICALERRORCODE) ;
    errorList.push_back(e) ;
  }

  return error ;
}

/**
 * \param vDevice - a vector of devices (description)
 * \param errorList - list of exceptions during the download
 * \param pllReset - force the PLL reset at the starting (default false)
 * \param dohSet - set the DOH (default true)
 * \return error number
 * \exception FecHandlerException
 */
unsigned int FecAccessManager::downloadValuesMultipleFrames ( deviceVector *vDevice, std::list<FecExceptionHandler *> &errorList, bool pllReset, bool dohSet ) throw (FecExceptionHandler) {

  if (displayDebugMessage_) 
    std::cout << __PRETTY_FUNCTION__ << ": " << std::dec << vDevice->size() << " devices to be downloaded => " << maxErrorAllowed_ << std::endl ;
  
  // A message
  char msg[MAXCHARDECODEKEY] ;

  // Number of errors
  unsigned int error = 0 ;  

  // vector of PLL to send the frames
  deviceVector pllVector ;
  
  if ( (vDevice != NULL) && (!vDevice->empty()) ) {

    // hash_map of list of device frames
    accessDeviceTypeListMap vAccessDevices ;

    // For each device => access it
    for (deviceVector::iterator device = vDevice->begin() ; (device != vDevice->end()) && (! haltStateMachine_) &&  ( (error < maxErrorAllowed_) || (maxErrorAllowed_ == 0) ); device ++) {

      deviceDescription *deviced = *device ;
      keyType indexFecRing = getFecRingKey ((*device)->getKey()) ;

      switch (deviced->getDeviceType()) {
      case APV25: {
	apvDescription *apvDevice = (apvDescription *)deviced ;
	unsigned int err = parseApv ( *apvDevice, errorList, false ) ;
	if (!err) // Each accesses is classified by ring
	  apvSet_[apvDevice->getKey()]->getBlockWriteValues(*apvDevice, vAccessDevices[indexFecRing]) ;
	else error += err ;
	break ;
      }
      case PLL: {
	pllDescription *pllDevice = (pllDescription *)deviced ;
	unsigned err = parsePll ( *pllDevice, errorList, false ) ;
	if (!err) { // Each accesses is classified by ring
	  try {
	    pllSet_[pllDevice->getKey()]->getBlockWriteValues(*pllDevice, vAccessDevices[indexFecRing]) ;
	  }
	  catch (FecExceptionHandler &e) { // in case of bad settings in PLL descriptions 
	    errorList.push_back(e.clone()) ;
	    error ++ ;
	  }
	}
	else error += err ;
	break ;
      }
      case DOH: {
	// Address DOHI2CADDRESS for DOH (defined in DohAccess.h)
	laserdriverDescription *laserdriverDevice = (laserdriverDescription *)deviced ;
	error += parseDoh ( *laserdriverDevice, errorList, dohSet ) ;
	break ;
      }
      case LASERDRIVER: {
	if (getAddressKey(deviced->getKey()) == DOHI2CADDRESS) {
	  // Address DOHI2CADDRESS for DOH (defined in DohAccess.h)
	  laserdriverDescription *laserdriverDevice = (laserdriverDescription *)deviced ;
	  error += parseDoh ( *laserdriverDevice, errorList, dohSet ) ;
	}
	else {
	  // Address 0x60 for laserdriver
	  laserdriverDescription *laserdriverDevice = (laserdriverDescription *)deviced ;
	  unsigned int err = parseLaserdriver ( *laserdriverDevice, errorList, false ) ;
	  if (!err) // Each accesses is classified by ring
	    laserdriverSet_[laserdriverDevice->getKey()]->getBlockWriteValues(*laserdriverDevice, vAccessDevices[indexFecRing]) ;
	  else error += err ;
	}
	break ;
      }
      case APVMUX: {
	muxDescription *muxDevice = (muxDescription *)deviced ;
	unsigned int err = parseMux ( *muxDevice, errorList, false ) ;
	if (!err) // Each accesses is classified by ring
	  muxSet_[muxDevice->getKey()]->getBlockWriteValues(*muxDevice, vAccessDevices[indexFecRing]) ;
	else error += err ;
	break ;
      }
      case PHILIPS: {
	philipsDescription *philipsDevice = (philipsDescription *)deviced ;
	unsigned int err = parsePhilips ( *philipsDevice, errorList, false ) ;
	if (!err) // Each accesses is classified by ring
	  philipsSet_[philipsDevice->getKey()]->getBlockWriteValues(*philipsDevice, vAccessDevices[indexFecRing]) ;
	break ;
      }
      case DCU: {
	dcuDescription *dcuDevice = (dcuDescription *)deviced ;
	// temporary exclusion of dcus on TIB ring 21.3 and TEC- ring 16.8
        if (
	    /*
	    (dcuDevice->getFecSlot()==21 && dcuDevice->getRingSlot()==3) 
	    ||
	    */
	    (dcuDevice->getCrateId()==3 &&dcuDevice->getFecSlot()==16 && dcuDevice->getRingSlot()==8)) {
	  std::cerr << "FecAccessManager::downloadValuesMultipleFrames: not parsing dcu 0x"<<std::hex<<dcuDevice->getDcuHardId() << std::dec<<" on crate "<<dcuDevice->getCrateId()<<" ring "<< dcuDevice->getFecSlot()<<"."<<dcuDevice->getRingSlot() << std::endl; 
	}
	else {
	error += parseDcu ( *dcuDevice, errorList, false ) ;
	}
	break ;
      }
#ifdef PRESHOWER
      case DELTA: {
	deltaDescription *deltaDevice = (deltaDescription *)deviced ;
	unsigned int err = parseDelta ( *deltaDevice, errorList, false ) ;
	if (!err) // Each accesses is classified by ring
	  deltaSet_[deltaDevice->getKey()]->getBlockWriteValues(*deltaDevice, vAccessDevices[indexFecRing]) ;
	else error += err ;
	break ;
      }
      case PACE: {
	paceDescription *paceDevice = (paceDescription *)deviced ;
	error += parsePace ( *paceDevice, errorList, true ) ;
	break ;
      }
      case KCHIP: {
	kchipDescription *kchipDevice = (kchipDescription *)deviced ;
	error += parseKchip ( *kchipDevice, errorList, true ) ;
	break ;
      }
      case GOH: {
	gohDescription *gohDevice = (gohDescription *)deviced ;
	error += parseGoh ( *gohDevice, errorList, true ) ;
	break ;
      }
#endif // PRESHOWER
	  
#ifdef TOTEM
      case VFAT: {
	vfatDescription *vfatDevice = (vfatDescription *)deviced ;
	error += parseVfat ( *vfatDevice, errorList, true ) ;
	break ;
      }

      case CCHIP: {
	totemCChipDescription *cchipDevice = (totemCChipDescription *)deviced ;
	error += parseCChip ( *cchipDevice, errorList, true ) ;
	break ;
      }

      case TBB: {
	totemBBDescription *tbbDevice = (totemBBDescription *)deviced ;
	error += parseTbb ( *tbbDevice, errorList, true ) ;
	break ;
      }
#endif // TOTEM
	  
      default:
	decodeKey (msg, deviced->getKey()) ;
	std::cerr << "Unknown device: 0x" << std::hex << deviced->getDeviceType() << " (" << msg << ")" << std::endl ;
      }
    }
    // ---------------------------------------------------------------------------------------------------------------------------
    // Apply a reset on the PLL if needed
    if (pllSet_.size() > 0) {

      std::list<keyType> pllErrorBefore, pllErrorAfter ;
      bool errorGoingBit = false ;
      pllAccess::setPllCheckGoingBitMultipleFrames ( *fecAccess_, errorList, pllSet_, 
      						     &errorGoingBit, pllErrorBefore, pllErrorAfter,
      //                                             ^is a going bit is not correct ?
      						     pllReset, true ) ;
      //                                             ^ Check the GOING bit to see if the reset is necessary
      //                                                       ^ Normal reset (true = Cold reset)
      //if (pllErrorAfter.size() == 0) std::cout << "All PLL locked" << std::endl ;

      for (std::list<keyType>::iterator it = pllErrorAfter.begin() ; it != pllErrorAfter.end() ; it ++) {

	FecExceptionHandler *e = NEWFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_PLLOPERATIONAL,
								      XDAQFEC_PLLOPERATIONAL_MSG,
								      ERRORCODE,
								      *it) ;
	errorList.push_back(e) ;
      }
    }

    // ---------------------------------------------------------------------------------------------------------------------------
    // Make the download and decode the errors
    error += fecAccess_->setBlockDevices( vAccessDevices, errorList ) ;

    // Read out the DCU for tests
    //deviceVector dcuVector ;
    //dcuAccess::getDcuValuesMultipleFrames ( *fecAccess_, dcuSet_, dcuVector) ;
    //FecFactory::display (dcuVector) ;
  }


  // Adding an error if we raised the maximum number of errors
  if ( (maxErrorAllowed_ > 0) && (error >= maxErrorAllowed_) ) {
    FecExceptionHandler *e = NEWFECEXCEPTIONHANDLER ( TOOMUCHERROR,
						      "Maximum number of errors (" + toString(maxErrorAllowed_) + ") reached",
						      CRITICALERRORCODE) ;
    errorList.push_back(e) ;
  }

  haltStateMachine_ = false ;

  lastOperationNumberErrors_ = error ;

  return (error) ;
}

/** Download the values only for certain APV registers on all APVs
 * \param apvValues - parameters to be set (description)
 * \param apvMode - register of the APV (true = set it, false do not set it)
 * \param latency - register of the APV (true = set it, false do not set it)
 * \param muxGain - register of the APV (true = set it, false do not set it)
 * \param ipre - register of the APV (true = set it, false do not set it)
 * \param ipcasc - register of the APV (true = set it, false do not set it)
 * \param ipsf - register of the APV (true = set it, false do not set it)
 * \param isha - register of the APV (true = set it, false do not set it)
 * \param issf - register of the APV (true = set it, false do not set it)
 * \param ipsp - register of the APV (true = set it, false do not set it)
 * \param imuxin - register of the APV (true = set it, false do not set it)
 * \param ical - register of the APV (true = set it, false do not set it)
 * \param ispare - register of the APV (true = set it, false do not set it) => not set
 * \param vfp - register of the APV (true = set it, false do not set it)
 * \param vfs - register of the APV (true = set it, false do not set it)
 * \param vpsp - register of the APV (true = set it, false do not set it)
 * \param cdrv - register of the APV (true = set it, false do not set it)
 * \param csel - register of the APV (true = set it, false do not set it)
 * \param apvError - register of the APV (true = set it, false do not set it) => not set
 * \warning a download must be performed before calling this method
 */
unsigned int FecAccessManager::downloadValuesMultipleFrames ( apvDescription apvValues,
							      std::list<FecExceptionHandler *> &errorList,
							      bool apvModeF, bool latencyF, bool muxGainF,
							      bool ipreF,    bool ipcascF,  bool ipsfF,
							      bool ishaF,    bool issfF,    bool ipspF,
							      bool imuxinF,  bool icalF,    bool ispareF,
							      bool vfpF,     bool vfsF,     bool vpspF,
							      bool cdrvF,    bool cselF,    bool apvErrorF ) {
  
#ifdef DEBUGMSGERROR
  std::cout << "Set " << apvSet_.size() << " APVs" << std::endl ;
  if (apvModeF) std::cout << __PRETTY_FUNCTION__ << "Set APV Mode " << (int)apvValues.getApvMode() << std::endl ;
  if (latencyF) std::cout << __PRETTY_FUNCTION__ << "Set APV latency " << (int)apvValues.getLatency() << std::endl ;
  if (ishaF) std::cout << __PRETTY_FUNCTION__ << "Set APV ISHA " << (int)apvValues.getIsha() << std::endl ;
  if (vfsF) std::cout << __PRETTY_FUNCTION__ << "Set APV VFS " << (int)apvValues.getVfs() << std::endl; 
  if (icalF) std::cout << __PRETTY_FUNCTION__ << "Set APV ICAL " << (int)apvValues.getIcal() << std::endl ;
  if (cdrvF) std::cout << __PRETTY_FUNCTION__ << "Set APV CDRV " << (int)apvValues.getCdrv() << std::endl ; 
  if (cselF) std::cout << __PRETTY_FUNCTION__ << "Set APV CSEL " << (int)apvValues.getCsel() << std::endl ;
#endif


  // Number of errors
  unsigned int error = 0 ;
  
  // hash_map of list of device frames
  accessDeviceTypeListMap vAccessDevices ;
  
  // For each APV in the map
  for (apvAccessedType::iterator p=apvSet_.begin();p!=apvSet_.end();p++) {

    // Retreive the device connected
    apvAccess *device = p->second ;

#ifdef DEBUGMSGERROR
    char msg[80] ;
    decodeKey(msg,device->getKey()) ;
    std::cout << "Set the APV on " << msg << std::endl ;
#endif

    // Values
    device->getBlockWriteValues(apvValues, vAccessDevices[getFecRingKey(device->getKey())],
				apvModeF, latencyF, muxGainF, ipreF, ipcascF, ipsfF,
				ishaF, issfF, ipspF, imuxinF, icalF, ispareF, vfpF,
				vfsF, vpspF, cdrvF, cselF, apvErrorF) ;
  }

  // ---------------------------------------------------------------------------------------------------------------------------
  // Make the download and decode the errors
  error += fecAccess_->setBlockDevices( vAccessDevices, errorList ) ;

  // Number of errors
  lastOperationNumberErrors_ = error ;

  return (error) ;
}

/** Download the values for all the laserdriver (AOH)
 * \param laserdriverValues - parameters to be set (description)
 * \warning a download must be performed before calling this method
 */
unsigned int FecAccessManager::downloadValuesMultipleFrames ( laserdriverDescription laserdriverValues,
							      std::list<FecExceptionHandler *> &errorList ) {

  // Number of errors
  unsigned int error = 0 ;
  
  // hash_map of list of device frames
  accessDeviceTypeListMap vAccessDevices ;
  
  // For each LASERDRIVER in the map
  for (laserdriverAccessedType::iterator p=laserdriverSet_.begin();p!=laserdriverSet_.end();p++) {

    // Retreive the device connected
    laserdriverAccess *device = p->second ;

    // Values
    device->getBlockWriteValues(laserdriverValues, vAccessDevices[getFecRingKey(device->getKey())] ) ;
  }

  // ---------------------------------------------------------------------------------------------------------------------------
  // Make the download and decode the errors
  error += fecAccess_->setBlockDevices( vAccessDevices, errorList ) ;

  // Number of errors
  lastOperationNumberErrors_ = error ;

  return (error) ;
}

/** Download the values for all the APV MUX 
 * \param muxValues - parameters to be set (description)
 * \warning a download must be performed before calling this method
 */
unsigned int FecAccessManager::downloadValuesMultipleFrames ( muxDescription muxValues,
							      std::list<FecExceptionHandler *> &errorList ) {

  // Number of errors
  unsigned int error = 0 ;
  
  // hash_map of list of device frames
  accessDeviceTypeListMap vAccessDevices ;
  
  // For each APV MUX in the map
  for (muxAccessedType::iterator p=muxSet_.begin();p!=muxSet_.end();p++) {

    // Retreive the device connected
    muxAccess *device = p->second ;

    // Values
    device->getBlockWriteValues(muxValues, vAccessDevices[getFecRingKey(device->getKey())] ) ;
  }

  // ---------------------------------------------------------------------------------------------------------------------------
  // Make the download and decode the errors
  error += fecAccess_->setBlockDevices( vAccessDevices, errorList ) ;

  // Number of errors
  lastOperationNumberErrors_ = error ;

  return (error) ;
}

/** Download the values for all the PLL
 * \param pllValues - parameters to be set (description)
 * \warning a download must be performed before calling this method
 */
unsigned int FecAccessManager::downloadValuesMultipleFrames ( pllDescription pllValues,
							      std::list<FecExceptionHandler *> &errorList ) {

  // Number of errors
  unsigned int error = 0 ;
  
  // hash_map of list of device frames
  accessDeviceTypeListMap vAccessDevices ;
  
  // For each APV PLL in the map
  for (pllAccessedType::iterator p=pllSet_.begin();p!=pllSet_.end();p++) {

    // Retreive the device connected
    pllAccess *device = p->second ;

    // Values
    device->getBlockWriteValues(pllValues, vAccessDevices[getFecRingKey(device->getKey())] ) ;
  }

  // ---------------------------------------------------------------------------------------------------------------------------
  // Make the download and decode the errors
  error += fecAccess_->setBlockDevices( vAccessDevices, errorList ) ;

  // Number of errors
  lastOperationNumberErrors_ = error ;

#ifdef DEBUGMSGERROR
  std::cout << "Set the delay fine to 0x" << std::hex << (int)pllValues.getDelayFine() << " and the delay coarse to " << (int)pllValues.getDelayCoarse() << std::endl ;
  pllAccess::displayPllRegisters (*fecAccess_, pllSet_) ;
#endif

  return (error) ;
}

/** Download the values for all the PLL by adding a certain delay to each PLL
 * \param delay - delay to be added for each PLL
 * \warning a download must be performed before calling this method
 */
unsigned int FecAccessManager::downloadValuesMultipleFrames ( tscType8 delay,
							      std::list<FecExceptionHandler *> &errorList ) {

  // Number of errors
  unsigned int error = 0 ;
  
  // hash_map of list of device frames
  accessDeviceTypeListMap vAccessDevices ;
  
  // For each PLL in the map
  for (pllAccessedType::iterator p=pllSet_.begin();p!=pllSet_.end();p++) {

    // Retreive the device connected
    pllAccess *device = p->second ;

    // Values
    device->getBlockWriteValues(delay, vAccessDevices[getFecRingKey(device->getKey())] ) ;
  }

  // ---------------------------------------------------------------------------------------------------------------------------
  // Make the download and decode the errors
  error += fecAccess_->setBlockDevices( vAccessDevices, errorList ) ;

  // Number of errors
  lastOperationNumberErrors_ = error ;

  return (error) ;
}

/** 
 * \param comparison - do the comparison between what you write is what your read
 * \param dcuUpload - upload the DCU (by default true)
 * \return all the values in a vector of description, NULL is the upload is interrupted
 * \warning delete the vector once you have finished with it
 * \warning if the comparison is set, only the differences will be stored in the database 
 * or file
 */
deviceVector *FecAccessManager::uploadValues ( std::list<FecExceptionHandler *> &errorList, bool comparison, bool dcuUpload, bool dohSet ) {

  // Retreive all the devices from the parsing class
  deviceVector *vDevice = new deviceVector ( ) ;
  
  unsigned int error = 0 ;
  unsigned int totalVal = 0 ;

  // Add all the devices
  for (apvAccessedType::iterator p=apvSet_.begin();(p!=apvSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    apvAccess *device = p->second ;

    apvDescription *apvDevice = NULL ;
    try {
      // Retreive the values
      apvDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	apvDescription *apvOrig = (apvDescription *)device->getDownloadedValues () ;
        if ( *apvOrig != *apvDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( apvDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the APV",
						 WARNINGCODE,
						 device->getKey()) ;
        }
	else
	  delete apvDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( apvDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an APV device"
      errorList.push_back (e.clone()) ;
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Add all the devices
  for (muxAccessedType::iterator p=muxSet_.begin();(p!=muxSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    muxAccess *device = p->second ;

    muxDescription *muxDevice = NULL ;
    try {
      // Retreive the values
      muxDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	muxDescription *muxOrig = (muxDescription *)device->getDownloadedValues () ;
        if ( *muxOrig != *muxDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( muxDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				     XDAQFEC_ERRORINCOMPARISON_MSG + " for the APV MUX",
				     WARNINGCODE,
				     device->getKey()) ;
        }
	else delete muxDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( muxDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of a MUX device"
      errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  if (dcuUpload) {
    // Add all the devices
    for (dcuAccessedType::iterator p=dcuSet_.begin();(p!=dcuSet_.end()) && (! haltStateMachine_);p++) {

      // Retreive the device connected
      dcuAccess *device = p->second ;

      dcuDescription *dcuDevice = NULL ;
      
      try {
	// Retreive the values
	dcuDevice = device->getValues() ;
	
	// Insert it in the vector vDevice
	vDevice->push_back ( dcuDevice ) ;
	
	totalVal += 1 ;
      }
      catch (FecExceptionHandler &e) {

	error ++ ;

	// "Error during the upload of a DCU device"
	errorList.push_back (e.clone()) ;	
	
#ifdef DEBUGGETREGISTERS
	// Check the registers of the FEC and CCU and display it
	if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
	// Original frame
	if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
	if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
	if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
      }
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }
  //#endif

  // Add all the devices
  for (laserdriverAccessedType::iterator p=laserdriverSet_.begin();(p!=laserdriverSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    laserdriverAccess *device = p->second ;

    laserdriverDescription *laserdriverDevice = NULL ;
    try {
      // Retreive the values
      laserdriverDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	laserdriverDescription *laserdriverOrig = (laserdriverDescription *)device->getDownloadedValues () ;
        if ( *laserdriverOrig != *laserdriverDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( laserdriverDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				     XDAQFEC_ERRORINCOMPARISON_MSG + " for the AOH",
				     WARNINGCODE,
				     device->getKey()) ;
        }
	else delete laserdriverDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( laserdriverDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of a laserdriver device"
      errorList.push_back (e.clone()) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Add all the devices
  for (dohAccessedType::iterator p=dohSet_.begin();(p!=dohSet_.end()) && (! haltStateMachine_) && (dohSet);p++) {

    // Retreive the device connected
    DohAccess *device = p->second ;

    laserdriverDescription *dohDevice = NULL ;
    try {
      // Retreive the values
      dohDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	laserdriverDescription *dohOrig = (laserdriverDescription *)device->getDownloadedValues () ;
        if ( *dohOrig != *dohDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( dohDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				     XDAQFEC_ERRORINCOMPARISON_MSG + " for the DOH",
				     WARNINGCODE,
				     device->getKey()) ;
        }
	else delete dohDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( dohDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an DOH device"
      errorList.push_back (e.clone()) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Add all the devices
  for (philipsAccessedType::iterator p=philipsSet_.begin();(p!=philipsSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    philipsAccess *device = p->second ;

    philipsDescription *philipsDevice = NULL ;
    try {
      // Retreive the values
      philipsDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	philipsDescription *philipsOrig = (philipsDescription *)device->getDownloadedValues () ;
        if ( *philipsOrig != *philipsDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( philipsDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				     XDAQFEC_ERRORINCOMPARISON_MSG + " for the philips",
				     WARNINGCODE,
				     device->getKey()) ;
        }
	else delete philipsDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( philipsDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an Philips device"
      errorList.push_back (e.clone()) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Add all the devices
  for (pllAccessedType::iterator p=pllSet_.begin();(p!=pllSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    pllAccess *device = p->second ;

    pllDescription *pllDevice = NULL ;
    try {
      // Retreive the values
      pllDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	pllDescription *pllOrig = (pllDescription *)device->getDownloadedValues () ;
        if ( *pllOrig != *pllDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( pllDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				     XDAQFEC_ERRORINCOMPARISON_MSG + " for the PLL",
				     WARNINGCODE,
				     device->getKey()) ;
        }
	else delete pllDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( pllDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an PLL device"
      errorList.push_back (e.clone()) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

#ifdef PRESHOWER
  // Add all the devices
  for (deltaAccessedType::iterator p=deltaSet_.begin();(p!=deltaSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    deltaAccess *device = p->second ;

    deltaDescription *deltaDevice = NULL ;
    try {
      // Retreive the values
      deltaDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	deltaDescription *deltaOrig = (deltaDescription *)device->getDownloadedValues () ;
        if ( *deltaOrig != *deltaDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( deltaDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				     XDAQFEC_ERRORINCOMPARISON_MSG + " for the DELTA",
				     WARNINGCODE,
				     device->getKey()) ;
        }
	else delete deltaDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( deltaDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an delta device"
      errorList.push_back (e.clone()) ; 

#  ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }


  // Add all the devices
  for (paceAccessedType::iterator p=paceSet_.begin();(p!=paceSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    paceAccess *device = p->second ;

    paceDescription *paceDevice = NULL ;
    try {
      // Retreive the values
      paceDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	paceDescription *paceOrig = (paceDescription *)device->getDownloadedValues () ;
        if ( *paceOrig != *paceDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( paceDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				     XDAQFEC_ERRORINCOMPARISON_MSG + " for the PaceAM",
				     WARNINGCODE,
				     device->getKey()) ;
        }
	else delete paceDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( paceDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an Pace device"
      errorList.push_back (e.clone()) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Add all the devices
  for (kchipAccessedType::iterator p=kchipSet_.begin();(p!=kchipSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    kchipAccess *device = p->second ;

    kchipDescription *kchipDevice = NULL ;
    try {
      // Retreive the values
      kchipDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	kchipDescription *kchipOrig = (kchipDescription *)device->getDownloadedValues () ;
        if ( *kchipOrig != *kchipDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( kchipDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				     XDAQFEC_ERRORINCOMPARISON_MSG + " for the Kchip",
				     WARNINGCODE,
				     device->getKey()) ;
        }
	else delete kchipDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( kchipDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an Kchip device"
      errorList.push_back (e.clone()) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }
#endif // PRESHOWER

#ifdef TOTEM
  // Add all the devices
  for (vfatAccessedType::iterator p=vfatSet_.begin();(p!=vfatSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    vfatAccess *device = p->second ;

    vfatDescription *vfatDevice = NULL ;
    try {
      // Retreive the values
      vfatDevice = device->getVfatValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	vfatDescription *vfatOrig = (vfatDescription *)device->getDownloadedValues () ;
        if ( *vfatOrig != *vfatDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( vfatDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				     XDAQFEC_ERRORINCOMPARISON_MSG + " for the VFAT",
				     WARNINGCODE,
				     device->getKey()) ;
        }
	else delete vfatDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( vfatDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an vfat device"
      errorList.push_back (e.clone()) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Add all the devices
  for (cchipAccessedType::iterator p=cchipSet_.begin();(p!=cchipSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    totemCChipAccess *device = p->second ;

    totemCChipDescription *cchipDevice = NULL ;
    try {
      // Retreive the values
      cchipDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	totemCChipDescription *cchipOrig = (totemCChipDescription *)device->getDownloadedValues () ;
        if ( *cchipOrig != *cchipDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( cchipDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				     XDAQFEC_ERRORINCOMPARISON_MSG + " for the CChip",
				     WARNINGCODE,
				     device->getKey()) ;
        }
	else delete cchipDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( cchipDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an cchip device"
      errorList.push_back (e.clone()) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Add all the devices
  for (tbbAccessedType::iterator p=tbbSet_.begin();(p!=tbbSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    totemBBAccess *device = p->second ;

    totemBBDescription *tbbDevice = NULL ;
    try {
      // Retreive the values
      tbbDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	totemBBDescription *tbbOrig = (totemBBDescription *)device->getDownloadedValues () ;
        if ( *tbbOrig != *tbbDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( tbbDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				     XDAQFEC_ERRORINCOMPARISON_MSG + " for the TBB",
				     WARNINGCODE,
				     device->getKey()) ;
        }
	else delete tbbDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( tbbDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an vfat device"
      errorList.push_back (e.clone()) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }
#endif // TOTEM

  // Check and update the database or a file ...
  if (totalVal == 0) {

    error ++ ;
  }
  else if (displayDebugMessage_) {
    // Debug reporting
    std::string debugReport = "Found " + toString(totalVal) + " devices for all the modules" ;
    std::cout << debugReport << std::endl ;
  }

  lastOperationNumberErrors_ = error ;

  return (vDevice) ;
}

/** 
 * \param comparison - do the comparison between what you write is what your read
 * \param dcuUpload - upload the DCU (by default true)
 * \return all the values in a vector of description, NULL is the upload is interrupted
 * \warning delete the vector once you have finished with it
 * \warning if the comparison is set, only the differences will be stored in the database 
 * or file
 */
deviceVector *FecAccessManager::uploadValuesMultipleFrames ( std::list<FecExceptionHandler *> &errorList, bool comparison, bool dcuUpload, bool dohSet, std::ostream & ss) {
  
  // Retreive all the devices from the parsing class
  deviceVector *vDevice = new deviceVector ( ) ;
  
  unsigned int error = 0 ;
  unsigned int totalVal = 0 ;

  // --------------------------------------------------------------- APV
  // Find all the values for the APV
  try {
    deviceVector apvVector ;
    error += apvAccess::getApvValuesMultipleFrames (*fecAccess_, apvSet_, apvVector, errorList )  ;
    totalVal += apvVector.size() ;

    // Analyse the answer for comparison
    for (deviceVector::iterator apvIt = apvVector.begin() ; apvIt != apvVector.end() ; apvIt ++) {

      apvDescription *apvDevice = (apvDescription *)(*apvIt) ;

      if (comparison) {

	try {
	  // Old Values != Values Uploaded
	  apvDescription *apvOrig = (apvDescription *)apvSet_[apvDevice->getKey()]->getDownloadedValues () ;

	  if ( *apvOrig != *apvDevice ) {
	    
	    // Insert it in the vector vDevice
	    vDevice->push_back ( apvDevice ) ;
	    
 	    apvOrig->displayDifferences (*apvDevice, ss) ;
	    	  
	    // Error in comparison
	    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				       XDAQFEC_ERRORINCOMPARISON_MSG + " for the APV",
				       WARNINGCODE,
				       apvDevice->getKey()) ;
	  }
	  else
	    delete apvDevice ;
	}
	catch (FecExceptionHandler &e) {

	  // "Error during the upload of an APV device"
	  errorList.push_back (e.clone()) ;
	}
      }
      else { // no comparison put all values inside the vector
	// Insert it in the vector vDevice
        vDevice->push_back ( apvDevice ) ;
      }      
    }
  }
  catch (FecExceptionHandler &e) {

    // "Error during the upload of an APV device"
    errorList.push_back (e.clone()) ;
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // --------------------------------------------------------------- MUX
  // Find all the values for the MUX
  try {
    deviceVector muxVector ;
    error += muxAccess::getMuxValuesMultipleFrames (*fecAccess_, muxSet_, muxVector, errorList )  ;
    totalVal += muxVector.size() ;

    // Analyse the answer for comparison
    for (deviceVector::iterator muxIt = muxVector.begin() ; muxIt != muxVector.end() ; muxIt ++) {

      muxDescription *muxDevice = (muxDescription *)(*muxIt) ;

      if (comparison) {

	try {
	  // Old Values != Values Uploaded
	  muxDescription *muxOrig = (muxDescription *)muxSet_[muxDevice->getKey()]->getDownloadedValues () ;

	  if ( *muxOrig != *muxDevice ) {
	    
	    // Insert it in the vector vDevice
	    vDevice->push_back ( muxDevice ) ;
 	    muxOrig->displayDifferences (*muxDevice, ss) ;	 
	    	  
	    // Error in comparison
	    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				       XDAQFEC_ERRORINCOMPARISON_MSG + " for the APV MUX",
				       WARNINGCODE,
				       muxDevice->getKey()) ;
	  }
	  else
	    delete muxDevice ;
	}
	catch (FecExceptionHandler &e) {

	  // "Error during the upload of an MUX device"
	  errorList.push_back (e.clone()) ;
	}
      }
      else { // no comparison put all values inside the vector
	// Insert it in the vector vDevice
        vDevice->push_back ( muxDevice ) ;
      }      
    }
  }
  catch (FecExceptionHandler &e) {

    // "Error during the upload of an MUX device"
    errorList.push_back (e.clone()) ;
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // --------------------------------------------------------------- PLL
  // Find all the values for the PLL
  try {
    deviceVector pllVector ;
    error += pllAccess::getPllValuesMultipleFrames (*fecAccess_, pllSet_, pllVector, errorList )  ;
    totalVal += pllVector.size() ;

    // Analyse the answer for comparison
    for (deviceVector::iterator pllIt = pllVector.begin() ; pllIt != pllVector.end() ; pllIt ++) {

      pllDescription *pllDevice = (pllDescription *)(*pllIt) ;

      if (comparison) {

	try {
	  // Old Values != Values Uploaded
	  pllDescription *pllOrig = (pllDescription *)pllSet_[pllDevice->getKey()]->getDownloadedValues () ;

	  if ( *pllOrig != *pllDevice ) {
	    
	    // Insert it in the vector vDevice
	    vDevice->push_back ( pllDevice ) ;

 	    pllOrig->displayDifferences (*pllDevice, ss) ;	 

	    // Error in comparison
	    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				       XDAQFEC_ERRORINCOMPARISON_MSG + " for the PLL",
				       WARNINGCODE,
				       pllDevice->getKey()) ;
	  }
	  else
	    delete pllDevice ;
	}
	catch (FecExceptionHandler &e) {

	  // "Error during the upload of a PLL device"
	  errorList.push_back (e.clone()) ;
	}
      }
      else { // no comparison put all values inside the vector
	// Insert it in the vector vDevice
        vDevice->push_back ( pllDevice ) ;
      }      
    }
  }
  catch (FecExceptionHandler &e) {

    // "Error during the upload of a PLL device"
    errorList.push_back (e.clone()) ;
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // --------------------------------------------------------------- LASERDRIVER
  // Find all the values for the LASERDRIVER
  try {
    deviceVector laserdriverVector ;
    error += laserdriverAccess::getLaserdriverValuesMultipleFrames (*fecAccess_, laserdriverSet_, laserdriverVector, errorList )  ;
    totalVal += laserdriverVector.size() ;

    // Analyse the answer for comparison
    for (deviceVector::iterator laserdriverIt = laserdriverVector.begin() ; laserdriverIt != laserdriverVector.end() ; laserdriverIt ++) {

      laserdriverDescription *laserdriverDevice = (laserdriverDescription *)(*laserdriverIt) ;

      if (comparison) {

	try {
	  // Old Values != Values Uploaded
	  laserdriverDescription *laserdriverOrig = (laserdriverDescription *)laserdriverSet_[laserdriverDevice->getKey()]->getDownloadedValues () ;

	  if ( *laserdriverOrig != *laserdriverDevice ) {
	    
	    // Insert it in the vector vDevice
	    vDevice->push_back ( laserdriverDevice ) ;

 	    laserdriverOrig->displayDifferences (*laserdriverDevice, ss) ;	    	  
	    // Error in comparison
	    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				       XDAQFEC_ERRORINCOMPARISON_MSG + " for the AOH",
				       WARNINGCODE,
				       laserdriverDevice->getKey()) ;
	  }
	  else
	    delete laserdriverDevice ;
	}
	catch (FecExceptionHandler &e) {

	  // "Error during the upload of a laserdriver device"
	  errorList.push_back (e.clone()) ;
	}
      }
      else { // no comparison put all values inside the vector
	// Insert it in the vector vDevice
        vDevice->push_back ( laserdriverDevice ) ;
      }      
    }
  }
  catch (FecExceptionHandler &e) {

    // "Error during the upload of a laserdriver device"
    errorList.push_back (e.clone()) ;
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // --------------------------------------------------------------- DOH
  // Find all the values for the DOH
  for (dohAccessedType::iterator p=dohSet_.begin();(p!=dohSet_.end()) && (! haltStateMachine_) && (dohSet);p++) {

    // Retreive the device connected
    DohAccess *device = p->second ;

    laserdriverDescription *dohDevice = NULL ;
    try {
      // Retreive the values
      dohDevice = device->getValues() ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	laserdriverDescription *dohOrig = (laserdriverDescription *)device->getDownloadedValues () ;

#ifdef DEBUGMSGERROR
	std::cout << "Uploading DOH " << std::endl ;
	dohDevice->display() ;
	if ( *dohOrig != *dohDevice ) std::cout << "Differences" << std::endl;
	else std::cout << "No differences" << std::endl ;
#endif

        if ( *dohOrig != *dohDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( dohDevice ) ;

	  dohOrig->displayDifferences (*dohDevice, ss) ;	    	  

          // Error in comparison
	    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				       XDAQFEC_ERRORINCOMPARISON_MSG + " for the DOH",
				       WARNINGCODE,
				       dohDevice->getKey()) ;
        }
	else delete dohDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( dohDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an DOH device"
      errorList.push_back (e.clone()) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // --------------------------------------------------------------- Philips
  // Find all the values for the PHILIPS
  try {
    deviceVector philipsVector ;
    error += philipsAccess::getPhilipsValuesMultipleFrames (*fecAccess_, philipsSet_, philipsVector, errorList )  ;
    totalVal += philipsVector.size() ;

    // Analyse the answer for comparison
    for (deviceVector::iterator philipsIt = philipsVector.begin() ; philipsIt != philipsVector.end() ; philipsIt ++) {

      philipsDescription *philipsDevice = (philipsDescription *)(*philipsIt) ;

      if (comparison) {

	try {
	  // Old Values != Values Uploaded
	  philipsDescription *philipsOrig = (philipsDescription *)philipsSet_[philipsDevice->getKey()]->getDownloadedValues () ;

	  if ( *philipsOrig != *philipsDevice ) {
	    
	    // Insert it in the vector vDevice
	    vDevice->push_back ( philipsDevice ) ;	    
	  
	    // Error in comparison
	    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				       XDAQFEC_ERRORINCOMPARISON_MSG + " for the philips",
				       WARNINGCODE,
				       philipsDevice->getKey()) ;
	  }
	  else
	    delete philipsDevice ;
	}
	catch (FecExceptionHandler &e) {

	  // "Error during the upload of a Philips device"
	  errorList.push_back (e.clone()) ;
	}
      }
      else { // no comparison put all values inside the vector
	// Insert it in the vector vDevice
        vDevice->push_back ( philipsDevice ) ;
      }      
    }
  }
  catch (FecExceptionHandler &e) {

    // "Error during the upload of a Philips device"
    errorList.push_back (e.clone()) ;
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // ------------------------------------------------------------- DCU
  if (dcuUpload) {

    // Read out the DCU for tests
    try {
      deviceVector dcuVector ;
      error += dcuAccess::getDcuValuesMultipleFrames (*fecAccess_, dcuSet_, dcuVector, errorList )  ;
      totalVal += dcuVector.size() ;
      
      // Analyse the answer for comparison
      for (deviceVector::iterator dcuIt = dcuVector.begin() ; dcuIt != dcuVector.end() ; dcuIt ++) {
	
      dcuDescription *dcuDevice = (dcuDescription *)(*dcuIt) ;
      
      // Add it to the vector
      vDevice->push_back ( dcuDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      // "Error during the upload of a DCU device"
      errorList.push_back (e.clone()) ;
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // ------------------------------------------------------------- PRESHOWER

  // --------------------------------------------------------------- DELTA
  // Find all the values for the delta chip
  try {
    deviceVector deltaVector ;
    error += deltaAccess::getDeltaValuesMultipleFrames (*fecAccess_, deltaSet_, deltaVector, errorList )  ;
    totalVal += deltaVector.size() ;

    // Analyse the answer for comparison
    for (deviceVector::iterator deltaIt = deltaVector.begin() ; deltaIt != deltaVector.end() ; deltaIt ++) {

      deltaDescription *deltaDevice = (deltaDescription *)(*deltaIt) ;

      if (comparison) {

	try {
	  // Old Values != Values Uploaded
	  deltaDescription *deltaOrig = (deltaDescription *)deltaSet_[deltaDevice->getKey()]->getDownloadedValues () ;

	  if ( *deltaOrig != *deltaDevice ) {
	    
	    // Insert it in the vector vDevice
	    vDevice->push_back ( deltaDevice ) ;
	    
 	    deltaOrig->displayDifferences (*deltaDevice, ss) ;
	    	  
	    // Error in comparison
	    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
				       XDAQFEC_ERRORINCOMPARISON_MSG + " for the DELTA",
				       WARNINGCODE,
				       deltaDevice->getKey()) ;
	  }
	  else
	    delete deltaDevice ;
	}
	catch (FecExceptionHandler &e) {

	  // "Error during the upload of a delta device"
	  errorList.push_back (e.clone()) ;
	}
      }
      else { // no comparison put all values inside the vector
	// Insert it in the vector vDevice
        vDevice->push_back ( deltaDevice ) ;
      }      
    }
  }
  catch (FecExceptionHandler &e) {

    // "Error during the upload of an APV device"
    errorList.push_back (e.clone()) ;
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Number of operations
  lastOperationNumberErrors_ = error ;

  // Just to display
  //FecFactory::display (*vDevice) ;

  return (vDevice) ;
}

/** Upload the DCU from the hardware
 * \param dcuValues - output DCU descriptions
 * \param errorList - return the list of errors
 * \param dcuHardIdOnly - readout only the DCU hard ID
 * \return the number of error
 * \warning delete the vector once you have finished with it
 */
unsigned int FecAccessManager::uploadValues ( deviceVector &dcuValues, std::list<FecExceptionHandler *> &errorList, bool dcuHardIdOnly ) {

  unsigned int error = 0 ;

  // Add all the devices
  for (dcuAccessedType::iterator p=dcuSet_.begin();(p!=dcuSet_.end()) && (! haltStateMachine_);p++) {
    
    // Retreive the device connected
    dcuAccess *device = p->second ;

    dcuDescription *dcuDevice = NULL ;
      
    try {
      if (dcuHardIdOnly) {
	// Retreive only the DCU hard id
	dcuDevice = new dcuDescription ( device->getKey(), time(NULL), device->getDcuHardId(), 0,0,0,0,0,0,0,0, device->getDcuType()) ;
      }
      else {
	// Retreive the values
	dcuDevice = device->getValues() ;
      }
      
      // Insert it in the vector vDevice
      dcuValues.push_back ( dcuDevice ) ;
    }
    catch (FecExceptionHandler &e) {
      
      error ++ ;
      
      // "Error during the upload of a DCU device"
      errorList.push_back (e.clone()) ;	
      
#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }
  //FecFactory::display (dcuVector) ;

  return (error) ;
}

/** Upload the DCU from the hardware
 * \param dcuValues - output DCU descriptions
 * \param errorList - return the error list
 * \param dcuHardIdOnly - readout only the DCU hard ID
 * \return the number of error
 * \warning delete the vector once you have finished with it
 */
unsigned int FecAccessManager::uploadValuesMultipleFrames ( deviceVector &dcuValues, std::list<FecExceptionHandler *> &errorList, bool dcuHardIdOnly ) {

  unsigned int error = 0 ;

  if (dcuSet_.size() > 0) {
    error = dcuAccess::getDcuValuesMultipleFrames ( *fecAccess_, dcuSet_, dcuValues, errorList, dcuHardIdOnly ) ;
    //FecFactory::display (dcuVector) ;
  }

  return (error) ;
}

/** Parse the DOM for an APV. This method take the different values in the DOM
 * and create an apvDescription that is sent via to the apvAccess class. Note
 * that if the APV does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param apvDevice - APV device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \return If an exception is done, an error is returned.
 * \exception FecExceptionHandler
 * \warning Note that the FEC/Ring/CCU/channel is used for several devices
 * \warning All the exceptions are catched in this method
 */
unsigned int FecAccessManager::parseApv ( apvDescription apvDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {

    std::ostringstream debugReport ;
    debugReport << "APV 0x" << std::hex << (uint)apvDevice.getFecSlot()
		<< ":0x" << (uint)apvDevice.getRingSlot()
		<< ":0x" << (uint)apvDevice.getCcuAddress()
		<< ":0x" << (uint)apvDevice.getChannel()
		<< ":0x" << (uint)apvDevice.getAddress()
		<< std::endl << "\t" << "muxgain :" << std::dec << (uint)apvDevice.getMuxGain() 
		<< std::endl << "\t" << "latency :"  << (uint)apvDevice.getLatency() 
		<< std::endl << "\t" << "mode :"  << (uint)apvDevice.getApvMode() 
		<< std::endl << "\t" << "isha :"  << (uint)apvDevice.getIsha() 
		<< std::endl << "\t" << "ipsp :"  << (uint)apvDevice.getIpsp() 
		<< std::endl << "\t" << "ipcasc :"  << (uint)apvDevice.getIpcasc()
		<< std::endl << "\t" << "vpsp :"  << (uint)apvDevice.getVpsp()  
		<< std::endl << "\t" << "ipsf :"  << (uint)apvDevice.getIpsf()  
		<< std::endl << "\t" << "ipre :"  << (uint)apvDevice.getIpre()  
		<< std::endl << "\t" << "imuxin :"  << (uint)apvDevice.getImuxin() 
		<< std::endl << "\t" << "vfp :"  << (uint)apvDevice.getVfp() 
		<< std::endl << "\t" << "issf :"  << (uint)apvDevice.getIssf()
		<< std::endl << "\t" << "vfs :"  << (uint)apvDevice.getVfs()
		<< std::endl << "\t" << "ical :"  << (uint)apvDevice.getIcal()
                // << std::endl << "\t" << "ispare :"  << (uint)apvDevice.getIspare()
		<< std::endl << "\t" << "cdrv :"  << (uint)apvDevice.getCdrv()
		<< std::endl << "\t" << "csel :"  << (uint)apvDevice.getCsel() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  apvAccess *apv = NULL ;
  keyType myKey = apvDevice.getKey() ;

  try { 

    // Try to find the Apv in the map
    apvAccessedType::iterator apvmap = apvSet_.find(myKey) ;
    
    if (apvmap == apvSet_.end()) {
      
      // Create the access
      apv = new apvAccess (fecAccess_, myKey) ;
      apv->setFecHardwareId (apvDevice.getFecHardwareId(),apvDevice.getCrateId()) ;
      
      // Add the access to the map
      apvSet_[apv->getKey()] = apv ;
    }
    else {
      
      apv = apvmap->second ;
    }

    // Set the values in the hardware
    if (setIt) {

      // Display
      if (displayDebugMessage_) 
	std::cout << "DEBUG Information: " << "APV: set the values ..." << std::endl ;

      apv->setValues (apvDevice) ;
    }    
    //else apv->setDescription (apvDevice) ; => will be done by the getBlockWriteFrames
  }
  catch (FecExceptionHandler &e) {

    // Manage errors to delete the previous access
    if ( (apv != NULL) && (apvSet_.find(myKey) == apvSet_.end()) ) delete apv ;
    
    error = 1 ;

    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of an APV: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }

    // Error report
    errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }
  
  return (error) ;
}

/** Retreive values from a device. 
 * This method take the different values
 * and create an pllDescription that is sent via to the pllAccess class. Note
 * that if the PLL does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param pllDevice - PLL device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \param pllReset - force the PLL reset at the starting (default false)
 * \return If an exception is done, an error is returned.
 * \exception FecExceptionHandler
 * \warning All the exceptions are catched in this method
 */
unsigned int FecAccessManager::parsePll ( pllDescription pllDevice, std::list<FecExceptionHandler *> &errorList, bool setIt, bool pllReset ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::ostringstream debugReport ;
    debugReport << "PLL 0x" << std::hex << (uint)pllDevice.getFecSlot()
		<< ":0x" << (uint)pllDevice.getRingSlot()
		<< ":0x" << (uint)pllDevice.getCcuAddress()
		<< ":0x" << (uint)pllDevice.getChannel()
		<< ":0x" << (uint)pllDevice.getAddress()
		<< std::endl << "\t" << "delay fine :" << std::dec << (uint)pllDevice.getDelayFine()
		<< std::endl << "\t" << "delay coarse :" << (uint)pllDevice.getDelayCoarse()
		<< std::endl << "\t" << "pllDac :" << (uint)pllDevice.getPllDac() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  pllAccess *pll = NULL ;
  keyType myKey = pllDevice.getKey() ;

  try { 

    // Try to find the Pll in the map
    pllAccessedType::iterator pllmap = pllSet_.find(myKey) ;
    if (pllmap == pllSet_.end()) {

      // Create the access
      pll = new pllAccess (fecAccess_, myKey) ;
      pll->setFecHardwareId (pllDevice.getFecHardwareId(),pllDevice.getCrateId()) ;

      // Add the access to the map
      pllSet_[pll->getKey()] = pll ;
    }
    else {
      
      pll = pllmap->second ;
    }

    // Set the PLL DAQ in the pllAccess
    pll->setPllDac(pllDevice.getPllDac()) ;

    // Set the values in the hardware
    if (setIt) {

      // Display
      if (displayDebugMessage_) 
	std::cout << "DEBUG Information: " << "PLL: set the values ..." << std::endl ;

      try {
	if (pllReset) {

#ifdef DEBUMSGERROR
	  char msg[80] ;
	  decodeKey(msg,pll->getKey()) ;
	  std::cout << "Making a PLL init for PLL " << msg << std::endl ;
#endif
	  pll->pllInit ( ) ;
	}
	//                              check  cold  value
	else pll->setPllCheckGoingBit ( false, true ) ;
      }
      catch (FecExceptionHandler &e) {

	if (!pll->getGoingStatus()) {
	  try {
	    // cold PLL reset
	    pll->setPllCheckGoingBit ( false, true ) ;
	    //                         ^ Check the going bit
	    //                                ^ PLL cold reset
	  }
	  catch (FecExceptionHandler &e) {

	    //#ifdef DEBUMSGERROR
	    if (!pll->getGoingStatus()) { 
	      char msgPll[80] ; decodeKey(msgPll,e.getHardPosition()) ;
	      std::cout << "PLL not locked for PLL on " << msgPll << std::endl ; 
	    }
	    //#endif

	    // Error display
	    if (displayDebugMessage_) {
	      std::cerr << "****************************** ERROR ***************************" << std::endl ;
	      std::cerr << "Error during a PLL reset, apply a cold PLL reset: " << e.what() << std::endl ;
	      std::cerr << "****************************************************************" << std::endl ;
	    }

	    // Error report
	    errorList.push_back (e.clone()) ;
	  }
	}
	else {
	  // Error report
	  errorList.push_back (e.clone()) ;
	}
      }

      pll->setValues (pllDevice) ;
    }
    else pll->setDescription (pllDevice) ;	
  }
  catch (FecExceptionHandler &e) {

    // Manage errors to delete the previous access
    if ( (pll != NULL) && (pllSet_.find(myKey) == pllSet_.end()) ) delete pll ;
    
    error = 1 ;

    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of a PLL: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }

    // Error report
    errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }

  return (error) ;
}

/** Parse the DOM for an MUX. This method take the different values in the DOM
 * and create an muxDescription that is sent via to the muxAccess class. Note
 * that if the MUX does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param muxDevice - MUX device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \return If an exception is done, an error is returned.
 * \exception FecExceptionHandler
 * \warning Note that the FEC/Ring/CCU/channel is used for several devices
 * \warning All the exceptions are catched in this method
 */
unsigned int FecAccessManager::parseMux ( muxDescription muxDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::ostringstream debugReport ;
    debugReport << "MUX 0x" << std::hex << (uint)muxDevice.getFecSlot()
		<< ":0x" << (uint)muxDevice.getRingSlot()
		<< ":0x" << (uint)muxDevice.getCcuAddress()
		<< ":0x" << (uint)muxDevice.getChannel()
		<< ":0x" << (uint)muxDevice.getAddress()
		<< std::endl << "\t" << "Resistor: " << std::dec << (uint)muxDevice.getResistor() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  muxAccess *mux = NULL ;
  keyType myKey = muxDevice.getKey() ;
    
  try {  

    // Try to find the Mux in the map
    muxAccessedType::iterator muxmap = muxSet_.find(myKey) ;
    if (muxmap == muxSet_.end()) {

      // Create the access
      mux = new muxAccess (fecAccess_, myKey) ;
      mux->setFecHardwareId (muxDevice.getFecHardwareId(),muxDevice.getCrateId()) ;

      // Add access to the map
      muxSet_[mux->getKey ()] = mux ;
    }
    else {
      
      mux = muxmap->second ;
    }
    
    // Set the values in the hardware
    if (setIt) {

      // Display
      if (displayDebugMessage_) 
	std::cout << "DEBUG Information: " << "MUX: set the values ..." << std::endl ;

      mux->setValues (muxDevice) ;
    }
    //else mux->setDescription (muxDevice) ; => will be done by the getBLockWriteFrames
  }
  catch (FecExceptionHandler &e) {

    // Manage errors to delete the previous access
    if ( (mux != NULL) && (muxSet_.find(myKey) == muxSet_.end()) ) delete mux ;

    error = 1 ;

    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of a MUX: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }

    // Error report
    errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }

  return (error) ;
}

/** Parse the DOM for a Laserdriver. This method take the different values in the DOM
 * and create an laserdriverDescription that is sent via to the laserdriverAccess class. Note
 * that if the Laserdriver does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param laserdriverDevice - laserdriver device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \return If an exception is done, an error is returned.
 * \exception FecExceptionHandler
 * \warning Note that the FEC/Ring/CCU/channel is used for several devices
 * \warning All the exceptions are catched in this method
 * \bug The old laserdriver have some problems and the error returned is not used
 */
unsigned int FecAccessManager::parseLaserdriver ( laserdriverDescription laserdriverDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::ostringstream debugReport ;
    debugReport << "Laserdriver 0x" << std::hex << (uint)laserdriverDevice.getFecSlot()
		<< ":0x" << (uint)laserdriverDevice.getRingSlot()
		<< ":0x" << (uint)laserdriverDevice.getCcuAddress()
		<< ":0x" << (uint)laserdriverDevice.getChannel()
		<< ":0x" << (uint)laserdriverDevice.getAddress()
		<< std::endl << "\t" << "Gain 0: " << std::dec << (uint)laserdriverDevice.getGain0()
		<< std::endl << "\t" << "Gain 1: " << (uint)laserdriverDevice.getGain1()
		<< std::endl << "\t" << "Gain 2: " << (uint)laserdriverDevice.getGain2()
		<< std::endl << "\t" << "Bias 0: " << (uint)laserdriverDevice.getBias0()
		<< std::endl << "\t" << "Bias 1: " << (uint)laserdriverDevice.getBias1()
		<< std::endl << "\t" << "Bias 2: " << (uint)laserdriverDevice.getBias2() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  laserdriverAccess *laserdriver = NULL ;
  keyType myKey = laserdriverDevice.getKey() ;

  try {    

    // Try to find the Laserdriver in the map
    laserdriverAccessedType::iterator laserdrivermap = laserdriverSet_.find(myKey) ;
    if (laserdrivermap == laserdriverSet_.end()) {

      // Create the access
      laserdriver = new laserdriverAccess (fecAccess_, myKey) ;
      laserdriver->setFecHardwareId (laserdriverDevice.getFecHardwareId(),laserdriverDevice.getCrateId()) ;

      // Add the access to the map
      laserdriverSet_[laserdriver->getKey ()] = laserdriver ;
    }
    else {
      
      laserdriver = laserdrivermap->second ;
    }
  
    // Set the values in the hardware
    if (setIt) {

      // Display
      if (displayDebugMessage_) 
	std::cout << "DEBUG Information: " << "Laserdriver: set the values ..." << std::endl ;

      laserdriver->setValues (laserdriverDevice) ;
    }
    //else laserdriver->setDescription (laserdriverDevice) ; => will be done by the getBlockWriteFrames
  }
  catch (FecExceptionHandler &e) {

    // Manage errors to delete the previous access
    if ( (laserdriver != NULL) && (laserdriverSet_.find(myKey) == laserdriverSet_.end()) ) delete laserdriver ;

    error = 1 ;

    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of an AOH: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }

    // Error report
    errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }

  return (error) ;
 }
 

/** Parse the DOM for a DOH. This method take the different values in the DOM
 * and create an laserdriverDescription that is sent via to the DohAccess class. Note
 * that if the Doh does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param laserdriverDevice - doh device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \return If an exception is done, an error is returned.
 * \exception FecExceptionHandler
 * \warning Note that the FEC/Ring/CCU/channel is used for several devices
 * \warning All the exceptions are catched in this method
 * \warning The DOH must be a laserdriver version 4.2
 */
unsigned int FecAccessManager::parseDoh ( laserdriverDescription dohDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {

    std::ostringstream debugReport ;
    debugReport << "DOH 0x" << std::hex << (uint)dohDevice.getFecSlot()
		<< ":0x" << (uint)dohDevice.getRingSlot()
		<< ":0x" << (uint)dohDevice.getCcuAddress()
		<< ":0x" << (uint)dohDevice.getChannel()
		<< ":0x" << (uint)dohDevice.getAddress()
		<< std::endl << "\t" << "Gain 0: " << std::dec << (uint)dohDevice.getGain0()
		<< std::endl << "\t" << "Gain 1: " << (uint)dohDevice.getGain1()
		<< std::endl << "\t" << "Gain 2: " << (uint)dohDevice.getGain2()
		<< std::endl << "\t" << "Bias 0: " << (uint)dohDevice.getBias0()
		<< std::endl << "\t" << "Bias 1: " << (uint)dohDevice.getBias1()
		<< std::endl << "\t" << "Bias 2: " << (uint)dohDevice.getBias2() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  DohAccess *doh = NULL ;
  keyType myKey = dohDevice.getKey() ;

  try {    

    // Try to find the Doh in the map
    dohAccessedType::iterator dohmap = dohSet_.find(myKey) ;
    if (dohmap == dohSet_.end()) {

      // Create the access
      doh = new DohAccess (fecAccess_, myKey) ;
      doh->setFecHardwareId (dohDevice.getFecHardwareId(),dohDevice.getCrateId()) ;

      // Add the access to the map
      dohSet_[doh->getKey ()] = doh ;
    }
    else {
      
      doh = dohmap->second ;
    }
  
    // Set the values in the hardware
    if (setIt) {

      // Display
      if (displayDebugMessage_) 
	std::cout << "DEBUG Information: " << "DOH: set the values ..." << std::endl ;

      doh->setValues (dohDevice, true) ;
    }
    else doh->setDescription (dohDevice) ;
  }
  catch (FecExceptionHandler &e) {

    // Manage errors to delete the previous access
    if ( (doh != NULL) && (dohSet_.find(myKey) == dohSet_.end()) ) delete doh ;

    error = 1 ;

    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of a DOH: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }

    // Error report
    errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }

  return (error) ;
 }
 

/** Parse the DOM for an Philips. This method take the different values in the DOM
 * and create an philipsDescription that is sent via to the philipsAccess class. Note
 * that if the Philips does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param philipsDevice - Philips device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \exception FecExceptionHandler
 * \warning Note that the FEC/Ring/CCU/channel is used for several devices
 * \warning All the exceptions are catched in this method
 */
unsigned int FecAccessManager::parsePhilips ( philipsDescription philipsDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::ostringstream debugReport ;
    debugReport << "Philips 0x" << std::hex << (uint)philipsDevice.getFecSlot()
		<< ":0x" << (uint)philipsDevice.getRingSlot()
		<< ":0x" << (uint)philipsDevice.getCcuAddress()
		<< ":0x" << (uint)philipsDevice.getChannel()
		<< ":0x" << (uint)philipsDevice.getAddress()
		<< std::endl << "\t" << "Register: " << std::dec << (uint)philipsDevice.getRegister() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  philipsAccess *philips = NULL;
  keyType myKey = philipsDevice.getKey() ;

  try {

    // Try to find the Philips in the map
    philipsAccessedType::iterator philipsmap = philipsSet_.find(myKey) ;
    if (philipsmap == philipsSet_.end()) {

      // Create the access
      philips = new philipsAccess (fecAccess_, myKey) ;
      philips->setFecHardwareId (philipsDevice.getFecHardwareId(),philipsDevice.getCrateId()) ;

      // Add the access to the map
      philipsSet_[philips->getKey ()] = philips ;
    }
    else {
      
      philips = philipsmap->second ;
    }
  
    // Set the values in the hardware
    if (setIt) {

      // Display
      if (displayDebugMessage_) 
	std::cout << "DEBUG Information: " << "Philips: set the values ..." << std::endl ;

      philips->setValues (philipsDevice) ;
    }
    //else philips->setDescription (philipsDevice) ; // => will be done by the getBlockWriteFrames
  }
  catch (FecExceptionHandler &e) {

    // Manage errors to delete the previous access
    if ( (philips != NULL) && (philipsSet_.find(myKey) == philipsSet_.end()) ) delete philips ;

    error = 1 ;

    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of a Philips: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }

    // Error report
    errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }

  return (error) ;
}

/** Parse the DOM for a DCU. This method take the different values in the DOM
 * and create an dcuDescription that is sent via to the dcuAccess class. Note
 * that if the DCU does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param dcuDevice - DCU device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \exception FecExceptionHandler
 * \warning Note that the FEC/Ring/CCU/channel is used for several devices
 * \warning All the exceptions are catched in this method
 */
unsigned int FecAccessManager::parseDcu ( dcuDescription dcuDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::ostringstream debugReport ;
    debugReport << "DCU 0x" << std::hex << (uint)dcuDevice.getFecSlot()
		<< ":0x" << (uint)dcuDevice.getRingSlot()
		<< ":0x" << (uint)dcuDevice.getCcuAddress()
		<< ":0x" << (uint)dcuDevice.getChannel()
		<< ":0x" << (uint)dcuDevice.getAddress() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  dcuAccess *dcu = NULL ;
  keyType myKey = dcuDevice.getKey() ;

  try { 

    // Try to find the dcu in the map
    dcuAccessedType::iterator dcumap = dcuSet_.find(myKey) ;
    if (dcumap == dcuSet_.end()) {
      // Create the access
      dcu = new dcuAccess (fecAccess_, myKey, dcuDevice.getDcuType()) ;
      dcu->setFecHardwareId (dcuDevice.getFecHardwareId(),dcuDevice.getCrateId()) ;

      // Add the access to the map
      dcuSet_[dcu->getKey ()] = dcu ;
    }
    else {
      
       dcu = dcumap->second ;
    }

    // Create the value to be set in the hardware
    // dcuDescription dcuD ( dcuDevice ) ;
  }
  catch (FecExceptionHandler &e) {

    // Manage errors to delete the previous access
    if ( (dcu != NULL) && (dcuSet_.find(myKey) == dcuSet_.end()) ) delete dcu ;

    error = 1 ;

    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of a DCU: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }

    // Error report
    errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }

  return (error) ;
}

/**
 * \param halt - boolean in order to interrupt the current operation
 * donwload or upload
 */
void FecAccessManager::setHalt (bool halt) {

  haltStateMachine_ = halt ;

}

/**
 * \return the halt state
 */
bool FecAccessManager::getHalt ( ) {

  return (haltStateMachine_) ;

}

/**
 * \param number - max number of error allowed
 */
void FecAccessManager::setMaxErrorAllowed ( unsigned int maxErrorAllowed ) {

  maxErrorAllowed_ = maxErrorAllowed ;
}

/**
 * \return max number of error allowed
 */
unsigned int FecAccessManager::getMaxErrorAllowed ( ) {

  return (maxErrorAllowed_) ;
}

/**
 * \return the number of errors during the last operation
 */
unsigned int FecAccessManager::getLastErrorLastOperation() {

  return (lastOperationNumberErrors_) ;
}

/**
 * Take a vector of PIA reset and reset all modules given by this vector
 * \param vPiaReset - vector of PIA reset description
 */
unsigned int FecAccessManager::resetPiaModules ( piaResetVector *vPiaReset, std::list<FecExceptionHandler *> &errorList ) throw (FecExceptionHandler) {

  // Number of errors
  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::string str = toString(vPiaReset->size()) + " PIA reset found" ;
    std::cout << "DEBUG Information: " << str << std::endl ;
  }
  
  if ( (vPiaReset != NULL) && (!vPiaReset->empty()) ) {

    // For each value => access it
    for (piaResetVector::iterator piaResetIt = vPiaReset->begin() ; (piaResetIt != vPiaReset->end()) && (! haltStateMachine_) && ( (error < maxErrorAllowed_) || (maxErrorAllowed_ == 0) ); piaResetIt ++) {  

      piaResetDescription *piaResetDes = *piaResetIt ;

      if (displayDebugMessage_) {
	std::ostringstream debugReport ;
	debugReport << "PIA reset for: 0x" << std::hex << (uint)piaResetDes->getFecSlot()
		    << ":0x" << (uint)piaResetDes->getRingSlot()
		    << ":0x" << (uint)piaResetDes->getCcuAddress()
		    << ":0x" << (uint)piaResetDes->getChannel()
		    << ":0x" << (uint)piaResetDes->getAddress()
		    << std::endl << "\t" << "Mask: " << std::dec << (uint)piaResetDes->getMask()
		    << std::endl << "\t" << "Active reset delay: " << (uint)piaResetDes->getDelayActiveReset()
		    << std::endl << "\t" << "Interval delay: " << (uint)piaResetDes->getIntervalDelayReset() ;
        std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
      }

      try {

        PiaResetAccess *piaReset ;

        // Try to find the access in the map
        keyType myKey = piaResetDes->getKey() ;
        piaAccessedType::iterator piamap = piaSet_.find(myKey) ;
        if (piamap == piaSet_.end()) {

          // Create the access
          piaReset = new PiaResetAccess ( fecAccess_, myKey, PIAINITIALVALUE) ;
	  piaReset->setFecHardwareId (piaResetDes->getFecHardwareId(),piaResetDes->getCrateId()) ;

          // Add access to the map
          piaSet_[piaReset->getKey()] = piaReset ;
        }
        else {
          piaReset = piamap->second ;
        }

	// Display
	if (displayDebugMessage_) 
	  std::cout << "DEBUG Information: " << "PIA reset: set the values ..." << std::endl ;

        // Set the values in the hardware
        piaReset->setValues (*piaResetDes) ;
      }
      catch (FecExceptionHandler &e) {
        
        error ++ ;

	// Error display
	if (displayDebugMessage_) {
	  std::cerr << "****************************** ERROR ***************************" << std::endl ;
	  std::cerr << "Error during reset through PIA/PIO reset: " << e.what() << std::endl ;
	  std::cerr << "****************************************************************" << std::endl ;
	}
	
	// Error report
	errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
	// Check the registers of the FEC and CCU and display it
	if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
	// Original frame
	if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
	if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
	if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
      }
    }
  }
  else {

    RAISEFECEXCEPTIONHANDLER ( NODATAAVAILABLE,
			       "No PIA reset values in the vector",
			       ERRORCODE ) ;
  }

  haltStateMachine_ = false ;

  lastOperationNumberErrors_ = error ;

  // Adding an error if we raised the maximum number of errors
  if ( (maxErrorAllowed_ > 0) && (error >= maxErrorAllowed_) ) {
    FecExceptionHandler *e = NEWFECEXCEPTIONHANDLER ( TOOMUCHERROR,
						      "Maximum number of errors (" + toString(maxErrorAllowed_) + ") reached",
						      CRITICALERRORCODE) ;
    errorList.push_back(e) ;
  }


  return (error) ;
}

/** Take a vector of PIA reset and reset all modules given by this vector and add all the PIA reset in the corresponding table
 * This method use a multiple frames algorithm to send in parallel on each CCU/PIA channel the reset. 
 * \param vPiaReset - vector of PIA reset description
 * \warning The mask must be the same
 * \warning The delayActiveReset and intervalDelayReset that will be used will be the maximum time given by this two arguments
 */
unsigned int FecAccessManager::resetPiaModulesMultipleFrames ( piaResetVector *vPiaReset, std::list<FecExceptionHandler *> &errorList ) throw (FecExceptionHandler) {

  // Number of errors
  unsigned int error = 0 ;
  unsigned long delayActiveReset   = 0 ; // max values of delayActiveReset
  unsigned long intervalDelayReset = 0 ; // max values of intervalDelayReset

  if (displayDebugMessage_) {
    std::string str = toString(vPiaReset->size()) + " PIA reset found" ;
    std::cout << "DEBUG Information: " << str << std::endl ;
  }    

  if ( (vPiaReset != NULL) && (!vPiaReset->empty()) ) {

    // For each value => access it
    for (piaResetVector::iterator piaResetIt = vPiaReset->begin() ; (piaResetIt != vPiaReset->end()) && (! haltStateMachine_) && ( (error < maxErrorAllowed_) || (maxErrorAllowed_ == 0) ); piaResetIt ++) {  

      piaResetDescription *piaResetDes = *piaResetIt ;

      // found the maximum delay
      delayActiveReset = (piaResetDes->getDelayActiveReset() > delayActiveReset) ? piaResetDes->getDelayActiveReset() : delayActiveReset ;
      intervalDelayReset = (piaResetDes->getIntervalDelayReset() > intervalDelayReset) ? piaResetDes->getIntervalDelayReset() : intervalDelayReset ;
      
      if (displayDebugMessage_) {
	std::ostringstream debugReport ;
	debugReport << "PIA reset for: 0x" << std::hex << (uint)piaResetDes->getFecSlot()
		    << ":0x" << (uint)piaResetDes->getRingSlot()
		    << ":0x" << (uint)piaResetDes->getCcuAddress()
		    << ":0x" << (uint)piaResetDes->getChannel()
		    << ":0x" << (uint)piaResetDes->getAddress()
		    << std::endl << "\t" << "Mask: " << std::dec << (uint)piaResetDes->getMask()
		    << std::endl << "\t" << "Active reset delay: " << (uint)piaResetDes->getDelayActiveReset()
		    << std::endl << "\t" << "Interval delay: " << (uint)piaResetDes->getIntervalDelayReset() ;
	std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
      }
      
      try {

        PiaResetAccess *piaReset ;

        // Try to find the access in the map
        keyType myKey = piaResetDes->getKey() ;
        piaAccessedType::iterator piamap = piaSet_.find(myKey) ;

        if (piamap == piaSet_.end()) {

          // Create the access
          piaReset = new PiaResetAccess ( fecAccess_, myKey, PIAINITIALVALUE) ;
	  piaReset->setFecHardwareId (piaResetDes->getFecHardwareId(),piaResetDes->getCrateId()) ;

          // Add access to the map
          piaSet_[piaReset->getKey()] = piaReset ;
        }
        else {
          piaReset = piamap->second ;
        }

	// store the settings that will be downloaded
	piaReset->setDescription (*piaResetDes) ;
      }
      catch (FecExceptionHandler &e) {
        
        error ++ ;

	// Error display
	if (displayDebugMessage_) {
	  std::cerr << "****************************** ERROR ***************************" << std::endl ;
	  std::cerr << "Error during reset through PIA/PIO reset: " << e.what() << std::endl ;
	  std::cerr << "****************************************************************" << std::endl ;
	}
	
	// Error report
	errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
	// Check the registers of the FEC and CCU and display it
	if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
	// Original frame
	if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
	if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
	if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
      }
    }

    try {
      // Send the PIA reset in multiple frame mode
      error += PiaResetAccess::resetPiaModulesMultipleFrame ( *fecAccess_, *vPiaReset, delayActiveReset, intervalDelayReset, PIAINITIALVALUE, errorList ) ;
    }
    catch (FecExceptionHandler &e) {
      
      error ++ ;

      // Error display
      if (displayDebugMessage_) {
	std::cerr << "****************************** ERROR ***************************" << std::endl ;
	std::cerr << "Error during reset through PIA/PIO reset: " << e.what() << std::endl ;
	std::cerr << "****************************************************************" << std::endl ;
      }

      // Error report
      errorList.push_back (e.clone()) ;
      
#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }
  else {

    RAISEFECEXCEPTIONHANDLER ( NODATAAVAILABLE,
			       "No PIA reset values in the vector",
			       ERRORCODE ) ;
  }

  haltStateMachine_ = false ;

  lastOperationNumberErrors_ = error ;

  // Adding an error if we raised the maximum number of errors
  if ( (maxErrorAllowed_ > 0) && (error >= maxErrorAllowed_) ) {
    FecExceptionHandler *e = NEWFECEXCEPTIONHANDLER ( TOOMUCHERROR,
						      "Maximum number of errors (" + toString(maxErrorAllowed_) + ") reached",
						      CRITICALERRORCODE) ;
    errorList.push_back(e) ;
  }

  return (error) ;
}

/** Reset the PLL through a specific method for cold conditions.
 * For each PLL connected, the reset is propagated
 * \param multiframes - use the multiframe to reset the PLL
 * \warning this method catch the exception
 */
unsigned int FecAccessManager::setColdPllReset ( std::list<FecExceptionHandler *> &errorList, bool multiFrames ) {

  // Number of errors
  unsigned int error = 0 ;

  // Reset PLL one by one
  if (! multiFrames) {
    // For each PLL
    for (pllAccessedType::iterator p=pllSet_.begin();p!=pllSet_.end();p++) {
      
      // Retreive the device connected
      pllAccess *device = p->second ;
      
      try {
	device->pllInit ( ) ;
      }
      catch (FecExceptionHandler &e) {
	
	if (displayDebugMessage_) {
	  char msg[80] ;
	  decodeKey (msg, device->getKey()) ;
	
	  std::cerr << "Problem in FecAccessManager::setColdPllReset when I use the cold reset " << msg << std::endl ;
	  std::cerr << e.what() << std::endl ;
	}

	// error list
	errorList.push_back (e.clone()) ;
	
#ifdef DEBUGGETREGISTERS
	// Check the registers of the FEC and CCU and display it
	if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
	// Original frame
	if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
	if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
	if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
      }
    }
  }
  else {

    // Send the reset in block frames
    // Apply a reset on the PLL if needed
    if (pllSet_.size() > 0) {
      std::list<keyType> pllErrorBefore, pllErrorAfter ;
      bool errorGoingBit = false ;
      pllAccess::setPllCheckGoingBitMultipleFrames ( *fecAccess_, errorList, pllSet_, 
						     &errorGoingBit, pllErrorBefore, pllErrorAfter,
      //                                             ^is a going bit is not correct ?
						     false, true ) ;
      //                                             ^ Check the GOING bit to see if the reset is necessary
      //                                                       ^ Normal reset (true = Cold reset)

      for (std::list<keyType>::iterator it = pllErrorAfter.begin() ; it != pllErrorAfter.end() ; it ++) {
	FecExceptionHandler *e = NEWFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_PLLOPERATIONAL,
								      XDAQFEC_PLLOPERATIONAL_MSG,
								      ERRORCODE,
								      *it) ;

	errorList.push_back(e) ;
      }
    }
  }

  haltStateMachine_ = false ;

  lastOperationNumberErrors_ = error ;

  return (error) ;
}

#ifdef PRESHOWER
/** Retreive values from a device. 
 * This method take the different values
 * and create an deltaDescription that is sent via to the deltaAccess class. Note
 * that if the DELTA does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param deltaDevice - DELTA device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \return If an exception is done, an error is returned.
 * \exception FecExceptionHandler
 * \warning All the exceptions are catched in this method
 */
unsigned int FecAccessManager::parseDelta ( deltaDescription deltaDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::ostringstream debugReport ;
    debugReport << "Delta 0x" << std::hex << (uint)deltaDevice.getFecSlot()
		<< ":0x" << (uint)deltaDevice.getRingSlot()
		<< ":0x" << (uint)deltaDevice.getCcuAddress()
		<< ":0x" << (uint)deltaDevice.getChannel()
		<< ":0x" << (uint)deltaDevice.getAddress() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  deltaAccess *delta = NULL ;
  keyType myKey = deltaDevice.getKey() ;

  try { 

    // Try to find the Delta in the map
    deltaAccessedType::iterator deltamap = deltaSet_.find(myKey) ;
    if (deltamap == deltaSet_.end()) {

      // Create the access
      delta = new deltaAccess (fecAccess_, myKey) ;
      delta->setFecHardwareId (deltaDevice.getFecHardwareId(),deltaDevice.getCrateId()) ;

      // Add the access to the map
      deltaSet_[delta->getKey()] = delta ;
    }
    else {
      
      delta = deltamap->second ;
    }

    //unsigned int ret = 0 ;
    bool ok = false ;
    //char clearMsg[1000] ;

    // Set the values in the hardware
    if (setIt) {

      // Display
      if (displayDebugMessage_)
	std::cout << "DEBUG Information: " << "Delta: set the values ..." << std::endl ;

      delta->setValues (deltaDevice) ;
      ok = true ;
    }
    else delta->setDescription (deltaDevice) ;
  }
  catch (FecExceptionHandler &e) {

    // Manage errors to delete the previous access
    if ( (delta != NULL) && (deltaSet_.find(myKey) == deltaSet_.end()) ) delete delta ;

    error = 1 ;
    
    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of a Delta: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }
    
    // Error report
    errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }

  return (error) ;
}

/** Retreive values from a device. 
 * This method take the different values
 * and create an paceDescription that is sent via to the paceAccess class. Note
 * that if the PACE does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param paceDevice - PACE device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \return If an exception is done, an error is returned.
 * \exception FecExceptionHandler
 * \warning All the exceptions are catched in this method
 */
unsigned int FecAccessManager::parsePace ( paceDescription paceDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::ostringstream debugReport ;
    debugReport << "Pace 0x" << std::hex << (uint)paceDevice.getFecSlot()
		<< ":0x" << (uint)paceDevice.getRingSlot()
		<< ":0x" << (uint)paceDevice.getCcuAddress()
		<< ":0x" << (uint)paceDevice.getChannel()
		<< ":0x" << (uint)paceDevice.getAddress() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  paceAccess *pace = NULL ;
  keyType myKey = paceDevice.getKey() ;

  try { 

    // Try to find the Pace in the map
    paceAccessedType::iterator pacemap = paceSet_.find(myKey) ;
    if (pacemap == paceSet_.end()) {

      // Create the access
      pace = new paceAccess (fecAccess_, myKey) ;
      pace->setFecHardwareId (paceDevice.getFecHardwareId(),paceDevice.getCrateId()) ;

      // Add the access to the map
      paceSet_[pace->getKey()] = pace ;
    }
    else {
      
      pace = pacemap->second ;
    }

    //unsigned int ret = 0 ;
    bool ok = false ;
    //char clearMsg[1000] ;

    // Set the values in the hardware
    if (setIt) {

      // Display
      if (displayDebugMessage_)
	std::cout << "DEBUG Information: " << "Pace: set the values ..." << std::endl ;

      pace->setValues (paceDevice) ;
      ok = true ;
    }
    else pace->setDescription (paceDevice) ;
  }
  catch (FecExceptionHandler &e) {

    // Manage errors to delete the previous access
    if ( (pace != NULL) && (paceSet_.find(myKey) == paceSet_.end()) ) delete pace ;
    
    error = 1 ;
    
    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of a Pace: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }
    
    // Error report
    errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }

  return (error) ;
}



/** Retreive values from a device. 
 * This method take the different values
 * and create an kchipDescription that is sent via to the kchipAccess class. Note
 * that if the KCHIP does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param kchipDevice - KCHIP device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \return If an exception is done, an error is returned.
 * \exception FecExceptionHandler
 * \warning All the exceptions are catched in this method
 */
unsigned int FecAccessManager::parseKchip ( kchipDescription kchipDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::ostringstream debugReport ;
    debugReport << "KChip 0x" << std::hex << (uint)kchipDevice.getFecSlot()
		<< ":0x" << (uint)kchipDevice.getRingSlot()
		<< ":0x" << (uint)kchipDevice.getCcuAddress()
		<< ":0x" << (uint)kchipDevice.getChannel()
		<< ":0x" << (uint)kchipDevice.getAddress() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  kchipAccess *kchip = NULL ;
  keyType myKey = kchipDevice.getKey() ;

  try { 

    // Try to find the Kchip in the map
    kchipAccessedType::iterator kchipmap = kchipSet_.find(myKey) ;
    if (kchipmap == kchipSet_.end()) {

      // Create the access
      kchip = new kchipAccess (fecAccess_, myKey) ;
      kchip->setFecHardwareId (kchipDevice.getFecHardwareId(),kchipDevice.getCrateId()) ;

      // Add the access to the map
      kchipSet_[kchip->getKey()] = kchip ;
    }
    else {
      
      kchip = kchipmap->second ;
    }

    //unsigned int ret = 0 ;
    bool ok = false ;
    //char clearMsg[1000] ;

    // Set the values in the hardware
    if (setIt) { 

      // Display
      if (displayDebugMessage_)
	std::cout << "DEBUG Information: " << "Kchip: set the values ..." << std::endl ;

      // Set the values in the hardware
      kchip->setValues (kchipDevice) ;
      ok = true ;
    }
    else kchip->setDescription (kchipDevice) ;
  }
  catch (FecExceptionHandler &e) {

    // Manage errors to delete the previous access
    if ( (kchip != NULL) && (kchipSet_.find(myKey) == kchipSet_.end()) ) delete kchip ;
    
    error = 1 ;
    
    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of a Kchip: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }
    
    // Error report
    errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }
  return (error) ;
}

/** Retreive values from a device. 
 * This method take the different values
 * and create an gohDescription that is sent via to the gohAccess class. Note
 * that if the GOH does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param gohDevice - GOH device with all informations
 * \return If an exception is done, an error is returned.
 * \exception FecExceptionHandler
 * \warning All the exceptions are catched in this method
 */
unsigned int FecAccessManager::parseGoh ( gohDescription gohDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::ostringstream debugReport ;
    debugReport << "Goh 0x" << std::hex << (uint)gohDevice.getFecSlot()
		<< ":0x" << (uint)gohDevice.getRingSlot()
		<< ":0x" << (uint)gohDevice.getCcuAddress()
		<< ":0x" << (uint)gohDevice.getChannel()
		<< ":0x" << (uint)gohDevice.getAddress() ;
    
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  gohAccess *goh = NULL ;
  keyType myKey = gohDevice.getKey() ;

  try { 

    // Try to find the Goh in the map
    gohAccessedType::iterator gohmap = gohSet_.find(myKey) ;
    if (gohmap == gohSet_.end()) {

      // Create the access
      goh = new gohAccess (fecAccess_, myKey) ;
      goh->setFecHardwareId (gohDevice.getFecHardwareId(),gohDevice.getCrateId()) ;

      // Add the access to the map
      gohSet_[goh->getKey()] = goh ;
    }
    else {
      
      goh = gohmap->second ;
    }

    //unsigned int ret = 0 ;
    bool ok = false ;
    //char clearMsg[1000] ;

    // Set the values in the hardware
    if (setIt) { 

      // Display
      if (displayDebugMessage_)
	std::cout << "DEBUG Information: " << "Goh: set the values ..." << std::endl ;

      goh->setValues (gohDevice) ;
      ok = true ;
    }
    else goh->setDescription (gohDevice) ;
  }
  catch (FecExceptionHandler &e) {

    // Manage errors to delete the previous access
    if ( (goh != NULL) && (gohSet_.find(myKey) == gohSet_.end()) ) delete goh ;
    
    error = 1 ;
    
    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of a GOH: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }

    // Error report
    errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }
  return (error) ;
}


#endif // PRESHOWER

#ifdef TOTEM
/** Retreive values from a device. 
 * This method take the different values
 * and create an vfatDescription that is sent via to the vfatAccess class. Note
 * that if the VFAT does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param vfatDevice - VFAT device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \return If an exception is done, an error is returned.
 * \exception FecExceptionHandler
 * \warning All the exceptions are catched in this method
 */
unsigned int FecAccessManager::parseVfat ( vfatDescription vfatDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::ostringstream debugReport ;
    debugReport << "Vfat 0x" << std::hex << (uint)vfatDevice.getFecSlot()
		<< ":0x" << (uint)vfatDevice.getRingSlot()
		<< ":0x" << (uint)vfatDevice.getCcuAddress()
		<< ":0x" << (uint)vfatDevice.getChannel()
		<< ":0x" << (uint)vfatDevice.getAddress() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  vfatAccess *vfat = NULL ;
  keyType myKey = vfatDevice.getKey() ;

  try {

    // Try to find the Vfat in the map
    vfatAccessedType::iterator vfatmap = vfatSet_.find(myKey) ;
    if (vfatmap == vfatSet_.end()) {

      // Create the access
      vfat = new vfatAccess (fecAccess_, myKey) ;
      vfat->setFecHardwareId (vfatDevice.getFecHardwareId(),vfatDevice.getCrateId()) ;

      // Add the access to the map
      vfatSet_[vfat->getKey()] = vfat ;
    }
    else {
      
      vfat = vfatmap->second ;
    }

    //unsigned int ret = 0 ;
    bool ok = false ;
    //char clearMsg[1000] ;

    // Set the values in the hardware
    if (setIt) {

      // Display
      if (displayDebugMessage_)
	std::cout << "DEBUG Information: " << "Vfat: set the values ..." << std::endl ;

      vfat->setVfatValues (vfatDevice) ;
      ok = true ;
    }
    else vfat->setVfatDescription (vfatDevice) ;
  }
  catch (FecExceptionHandler &e) {

    // Manage errors to delete the previous access
    if ( (vfat != NULL) && (vfatSet_.find(myKey) == vfatSet_.end()) ) delete vfat ;

    error = 1 ;
    
    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of a Vfat: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }
    
    // Error report
    errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }

  return (error) ;
}

/** Retreive values from a device. 
 * This method take the different values
 * and create an totemCChipDescription that is sent via to the totemCChipAccess class. Note
 * that if the CCHIP does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param cchipDevice - CCHIP device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \return If an exception is done, an error is returned.
 * \exception FecExceptionHandler
 * \warning All the exceptions are catched in this method
 */
unsigned int FecAccessManager::parseCChip ( totemCChipDescription cchipDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::ostringstream debugReport ;
    debugReport << "Cchip 0x" << std::hex << (uint)cchipDevice.getFecSlot()
		<< ":0x" << (uint)cchipDevice.getRingSlot()
		<< ":0x" << (uint)cchipDevice.getCcuAddress()
		<< ":0x" << (uint)cchipDevice.getChannel()
		<< ":0x" << (uint)cchipDevice.getAddress() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  totemCChipAccess *cchip = NULL ;
  keyType myKey = cchipDevice.getKey() ;

  try {

    // Try to find the Cchip in the map
    cchipAccessedType::iterator cchipmap = cchipSet_.find(myKey) ;
    if (cchipmap == cchipSet_.end()) {

      // Create the access
      cchip = new totemCChipAccess (fecAccess_, myKey) ;
      cchip->setFecHardwareId (cchipDevice.getFecHardwareId(),cchipDevice.getCrateId()) ;

      // Add the access to the map
      cchipSet_[cchip->getKey()] = cchip ;
    }
    else {
      
      cchip = cchipmap->second ;
    }

    //unsigned int ret = 0 ;
    bool ok = false ;
    //char clearMsg[1000] ;

    // Set the values in the hardware
    if (setIt) {

      // Display
      if (displayDebugMessage_)
	std::cout << "DEBUG Information: " << "Cchip: set the values ..." << std::endl ;

      cchip->setValues (cchipDevice) ;
      ok = true ;
    }
    else cchip->setDescription (cchipDevice) ;
  }
  catch (FecExceptionHandler &e) {

    // Manage errors to delete the previous access
    if ( (cchip != NULL) && (cchipSet_.find(myKey) == cchipSet_.end()) ) delete cchip ;

    error = 1 ;
    
    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of a Cchip: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }
    
    // Error report
    errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }

  return (error) ;
}

/** Retreive values from a device. 
 * This method take the different values
 * and create an totemBBDescription that is sent via to the totemBBAccess class. Note
 * that if the TBB does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param tbbDevice - TBB device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \return If an exception is done, an error is returned.
 * \exception FecExceptionHandler
 * \warning All the exceptions are catched in this method
 */
unsigned int FecAccessManager::parseTbb ( totemBBDescription tbbDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::ostringstream debugReport ;
    debugReport << "Tbb 0x" << std::hex << (uint)tbbDevice.getFecSlot()
		<< ":0x" << (uint)tbbDevice.getRingSlot()
		<< ":0x" << (uint)tbbDevice.getCcuAddress()
		<< ":0x" << (uint)tbbDevice.getChannel()
		<< ":0x" << (uint)tbbDevice.getAddress() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  totemBBAccess *tbb = NULL ;
  keyType myKey = tbbDevice.getKey() ;

  try {

    // Try to find the Tbb in the map
    tbbAccessedType::iterator tbbmap = tbbSet_.find(myKey) ;
    if (tbbmap == tbbSet_.end()) {

      // Create the access
      tbb = new totemBBAccess (fecAccess_, myKey) ;
      tbb->setFecHardwareId (tbbDevice.getFecHardwareId(),tbbDevice.getCrateId()) ;

      // Add the access to the map
      tbbSet_[tbb->getKey()] = tbb ;
    }
    else {
      
      tbb = tbbmap->second ;
    }

    //unsigned int ret = 0 ;
    bool ok = false ;
    //char clearMsg[1000] ;

    // Set the values in the hardware
    if (setIt) {

      // Display
      if (displayDebugMessage_)
	std::cout << "DEBUG Information: " << "Tbb: set the values ..." << std::endl ;

      tbb->setValues (tbbDevice) ;
      ok = true ;
    }
    else tbb->setDescription (tbbDevice) ;
  }
  catch (FecExceptionHandler &e) {

    // Manage errors to delete the previous access
    if ( (tbb != NULL) && (tbbSet_.find(myKey) == tbbSet_.end()) ) delete tbb ;

    error = 1 ;
    
    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of a Tbb: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }
    
    // Error report
    errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }

  return (error) ;
}
#endif // TOTEM
