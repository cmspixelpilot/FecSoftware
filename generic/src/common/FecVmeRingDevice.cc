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
*/

//#define FECVMERINGDEVICE_DEBUG
#ifdef DEBUGMSGERROR
#  define FECVMERINGDEVICE_DEBUG
#endif

//#define LOGRELOADFIRMARE "/tmp/reloadfirmware.txt"
#ifdef LOGRELOADFIRMARE
#include <fstream>
#endif

#include <stdio.h>
#include <iostream>
#include <iomanip>


#include <unistd.h> // for usleep

#include "tscTypes.h"
#include "keyType.h"
#include "stringConv.h"
#include "cmdDescription.h"
#include "dderrors.h"
#include "FecVmeRingDevice.h"

/** Bus adapter that must be created once
 */
pthread_mutex_t mutexBusAdapter_=PTHREAD_MUTEX_INITIALIZER ;
pthread_mutex_t mutexCrateReset_=PTHREAD_MUTEX_INITIALIZER ;
pthread_mutex_t mutexGetRingSr0Counter_=PTHREAD_MUTEX_INITIALIZER ;
pthread_mutex_t mutexSetFifoTransmitCounter_=PTHREAD_MUTEX_INITIALIZER ;
static bool crateResetDone= false;
static unsigned long getRingSr0Counter_=0;
static unsigned long setFifoTransmitCounter_=0;
static unsigned long setFifoTransmitSent_=0;

Sgi::hash_map<uint32_t, HAL::VMEBusAdapterInterface *> FecVmeRingDevice::busAdapter_ ;
HAL::ASCIIFileModuleMapper *FecVmeRingDevice::moduleMapper_ = NULL ;
HAL::ASCIIFileAddressTableContainer *FecVmeRingDevice::addressTableContainer_ = NULL ;
HAL::VMEAddressTableASCIIReader *FecVmeRingDevice::addressTableReader_ = NULL ;
HAL::VMEAddressTable *FecVmeRingDevice::addressTable_ = NULL ;

/******************************************************
		CONSTRUCTOR - DESTRUCTOR
******************************************************/

/** Create a FecVmeRingDevice that can manage the access to a FEC VME ring
 * \param vmeDevice: VME device from hal for the hardware access
 * \param fecSlot - slot of the FEC
 * \param ring - ring slot
 * \param init - initialise the FEC (with reset at the starting)
 * \warning note that the numbering schema comes from the file use for the creation of the bus adapter, see the configurePLugAndPlay and configureHardBaseAddress method from this class
 */
FecVmeRingDevice::FecVmeRingDevice (HAL::VME64xDevice *vmeDevice, tscType8 fecSlot, tscType8 ringSlot, bool init, fecRingDeviceFifoAccessModeEnum blockMode, uint32_t adapterSlot, bool invertClockPolarity ) throw ( FecExceptionHandler ) :
  FecRingDevice ( fecSlot, ringSlot, FECVME ) {

  // Raise an exception if the VME Device is incoherent
  if (vmeDevice == NULL) {
    
    // errorCode,errorMessage,faultSeverity,hardPosition
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION(HAL_BUSADAPTEREXCEPTION,
					  "The VME Device fails to be created",
					  FATALERRORCODE,
					  buildFecRingKey(fecSlot,ringSlot)) ;
  }

  // -----------------------------
  mFecInitialised_ = -1 ; // no hardware creation

#ifdef FECVMERINGDEVICE_DEBUG
  std::cout << "Here is the constructor of FecVmeRingDevice class. I am instance : " << std::hex << this << std::dec << std::endl;
  std::cout << "I will work with the VMEBOARD setted at : 0x" << std::hex << vmeDevice << std::dec << std::endl;
  std::cout << "\t for the ring " << std::dec << (int)fecSlot << ", ring " << (int)ringSlot << std::endl ;
#endif

  currentVmeBoard_ = vmeDevice;

  // Copy the item to be accessed
  //memcpy (fecHalItem_, FECHALITEM) ;
  for (int i = FECHALCONTROL0 ; i <= FECHALREC_FIFO_W_BL ; i ++) {
    strcpy (fecHalItem_[i], FECHALITEM[i]) ;
    fecHalItem_[i][FECHALCHARITEM[i]] = getRingSlot() + NUMBERASCIIDIFF ;    
  }

  // Read the FEC hardware id
  if(busAdapter_.empty())
    fecHardwareId_ = FecVmeRingDevice::getSerialNumber () ;
  else
    fecHardwareId_ = FecVmeRingDevice::getSerialNumber ( fecSlot, adapterSlot) ;
  
#ifdef FECVMERINGDEVICE_DEBUG
  std::cout << "Serial number is: " << fecHardwareId_ << std::endl;
  try{
    uint32_t VERS = getVmeVersion();
    std::cout << "Version=0x" << std::hex << VERS << std::endl;
  } catch (FecExceptionHandler &e) {
    std::cout <<" CCS::FE_Manager: Create FecAccess: FecExeption: "<<e.what()<< std::endl;
  } catch ( HAL::HardwareAccessException& e ) {
    std::cout << "*** Exception occurred : " << std::endl;
    std::cout << e.what() << std::endl;
  }
#endif

  // Disable IRQ
  setIRQ (false) ;

  // by default do not use BLT mode : 
  blockMode_ = blockMode ; // do not use block transfer

  mFecInitialised_ = 0 ; // hardware creation done but mFEC not initialised
  try {
    // Initialise or not the FEC
    FecRingDevice::setInitFecRingDevice ( init, invertClockPolarity ) ;
    mFecInitialised_ = 1 ; // hardware creation done and mFEC initialised
  }
  catch (FecExceptionHandler &e) {
#ifdef FECVMERINGDEVICE_DEBUG
    std::cerr << "Unable to initialise the ring on the ring " << std::dec << (int)fecSlot << "." << (int)ringSlot << ": " << e.what() << std::endl ;
#endif
    mFecInitialised_ = 0 ; // hardware creation done but mFEC not initialised
  }
}


/** Nothing
 * \warning the VME device used must be destroy by the creator (ie. FecAccess class in the current implementation). If you don't use the FecAcess, you must delete it).
 */
FecVmeRingDevice::~FecVmeRingDevice ( ) throw ( FecExceptionHandler ) {
}

/** \return the FEC hardware id
 */
std::string FecVmeRingDevice::getFecHardwareId ( ) {

  return fecHardwareId_ ;
}

/******************************************************
	CONTROL & STATUS RTEGISTERS ACCESS
******************************************************/

/* Write the value given as parameter in FEC control 0 register 
 * \param ctrl0Value - value of the CR0
 * \param force - if force is set then the value is applied blindly. if force is not set then the invert clock polarity is managed following the parameter in the class.
 */
void FecVmeRingDevice::setFecRingCR0 ( tscType16 ctrl0Value, bool force ) throw ( FecExceptionHandler ) {

  if (!force) {
    if (invertClockPolarity_) ctrl0Value |= FEC_CR0_POLARITY ;
    else ctrl0Value &= (~FEC_CR0_POLARITY) ;
  }

#ifdef FECVMERINGDEVICE_DEBUG
  std::cout << "DEBUG : writing value 0x" << std::hex << ctrl0Value << std::endl ;
#endif

  try {
    currentVmeBoard_->unmaskedWrite(fecHalItem_[FECHALCONTROL0], (uint32_t)ctrl0Value);
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
}


/** Reads a value from FEC ctrl0 
 *\return the value read
 */
tscType16 FecVmeRingDevice::getFecRingCR0( ) throw ( FecExceptionHandler ) {

  uint32_t ctrl0Value;
  try {

    currentVmeBoard_->unmaskedRead(fecHalItem_[FECHALCONTROL0], (haltype *)&ctrl0Value);


#ifdef FECVMERINGDEVICE_DEBUG
    std::cout << "DEBUG : reading value 0x" << std::hex << ctrl0Value << " from CRO" << std::endl ;
#endif
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return (tscType16)ctrl0Value;
}



/** Write the value given as parameter in FEC control 1 register 
 * \param value to be written
 */
void FecVmeRingDevice::setFecRingCR1( tscType16 ctrl1Value ) throw ( FecExceptionHandler ) {

#ifdef FECVMERINGDEVICE_DEBUG
  std::cout << "DEBUG : writing value 0x" << std::hex << ctrl1Value << " in CR1" << std::endl ;
#endif

  try {
    currentVmeBoard_->unmaskedWrite(fecHalItem_[FECHALCONTROL1], (uint32_t)ctrl1Value);
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
}



/** Reads a value from FEC ctrl1 
 * \return value of the FEC CR1 for that ring
 */
tscType16 FecVmeRingDevice::getFecRingCR1( ) throw ( FecExceptionHandler ) {

  uint32_t ctrl1Value;
  
  try { 
    currentVmeBoard_->unmaskedRead(fecHalItem_[FECHALCONTROL1], (haltype *)&ctrl1Value);

#ifdef FECVMERINGDEVICE_DEBUG
    std::cout << "DEBUG : reading value 0x" << std::hex << ctrl1Value << " from CR1" << std::endl ;
#endif
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return (tscType16)ctrl1Value;
}


/** Reads a value from FEC SR0 
 * \return the value of the FEC SR0
 */
tscType32 FecVmeRingDevice::getFecRingSR0(unsigned long sleeptime) throw ( FecExceptionHandler ) {

  uint32_t sr0Value;
  
#ifdef FECVMERINGDEVICE_DEBUG
  std::cout << "FecVmeRingDevice::getFecRingSR0: Requesting Item : " << fecHalItem_[FECHALSTATUS0] << " for VMEBOARD : " << std::hex << currentVmeBoard_ << std::endl;
  std::cout << "FecVmeRingDevice::getFecRingSR0: Meaning : I will work on FecSlot " << std::dec << (int)getFecSlot() << ", Ring " << (int)getRingSlot() << std::endl;
#endif
  
  try {
    currentVmeBoard_->unmaskedRead(fecHalItem_[FECHALSTATUS0], (haltype *)&sr0Value);
#ifdef FECVMERINGDEVICE_DEBUG
    std::cout << "DEBUG : reading value 0x" << std::hex << sr0Value << " from SRO" << std::endl ;
#endif
  }  
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  
#ifdef FECVMERINGDEVICE_DEBUG
  std::cout << "value read from SRO is : " << std::hex << sr0Value << std::endl << std::endl;
#endif
  if (sleeptime) {
  // sleep some time
  struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = sleeptime*1000000;
  nanosleep (&req,NULL) ;
  }

  pthread_mutex_lock(&mutexGetRingSr0Counter_) ;
  getRingSr0Counter_++;    
  pthread_mutex_unlock(&mutexGetRingSr0Counter_) ;

  return (tscType32)sr0Value;
}


/** Reads a value from FEC SR1
 * \return the value of the FEC SR1
 */
tscType16 FecVmeRingDevice::getFecRingSR1( ) throw ( FecExceptionHandler ) {

  uint32_t sr1Value;

  try { 
    currentVmeBoard_->unmaskedRead(fecHalItem_[FECHALSTATUS1], (haltype *)&sr1Value);

#ifdef FECVMERINGDEVICE_DEBUG
    std::cout << "DEBUG : reading value 0x" << std::hex << sr1Value << " from SR1" << std::endl ;
#endif
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return (tscType16)sr1Value;
}

/** Return the version of the firmware
 * \return Firmware version
 * \warning the value read is mask with (0xFF00 >> 16)
 */
tscType16 FecVmeRingDevice::getFecFirmwareVersion( ) throw ( FecExceptionHandler ) {

  //std::cout << fecHalItem_[FECHALVERSION_SRC] << " ring initialised " << getmFecInitialised() << std::endl ;

  uint32_t fecVersion;

  try {
    currentVmeBoard_->unmaskedRead(fecHalItem_[FECHALVERSION_SRC], (haltype *)&fecVersion);
    fecVersion = (fecVersion >> 8) & 0xFF ;

#ifdef FECVMERINGDEVICE_DEBUG
    std::cout << "DEBUG : Firmware version: 0x" << std::hex << fecVersion << " from FEC" << std::endl ;
#endif
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return (tscType16)fecVersion;
}


/******************************************************
	FIFO ACCESS - NATIVE 32 BITS FORMAT
	NATIVE FORMAT ACCESS ALLOWS R/W OPERATIONS
******************************************************/

/** Return a word from the FIFO receive
 * \param return a word from the FIFO receive
 */
tscType32 FecVmeRingDevice::getFifoReceive ( ) throw ( FecExceptionHandler ) {

  uint32_t fiforec_value;

  try {
    currentVmeBoard_->unmaskedRead(fecHalItem_[FECHALREC_FIFO_R], (haltype *)&fiforec_value);

#ifdef FECVMERINGDEVICE_DEBUG
    std::cout << "Value 0x" << std::hex << fiforec_value << " read from fifo receive" << std::endl ;
#endif
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return (tscType32)fiforec_value;
}

/** set a word in the FIFO receive
 * \param fiforecValue - word to be set
 */
void FecVmeRingDevice::setFifoReceive( tscType32 fiforecValue ) throw ( FecExceptionHandler ) {

  try {
    currentVmeBoard_->unmaskedWrite(fecHalItem_[FECHALREC_FIFO_W], (uint32_t)fiforecValue);

#ifdef FECVMERINGDEVICE_DEBUG
    std::cout << "DEBUG : writing value 0x" << std::hex << fiforecValue << " to fifo receive" << std::endl ;
#endif
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

}

/** Return a word from the FIFO return
 * \param return a word from the FIFO return
 */
tscType8 FecVmeRingDevice::getFifoReturn( ) throw ( FecExceptionHandler ) {

  uint32_t fiforet_value;

  try {
    currentVmeBoard_->unmaskedRead(fecHalItem_[FECHALRET_FIFO_R], (haltype *)&fiforet_value);

#ifdef FECVMERINGDEVICE_DEBUG
    std::cout << "Value 0x" << std::hex << fiforet_value << " read from fifo return" << std::endl ;
#endif
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return (tscType32)fiforet_value;
}

/** set a word in the FIFO return
 * \param fiforetValue - word to be set
 */
void FecVmeRingDevice::setFifoReturn( tscType8 fiforetValue ) throw ( FecExceptionHandler ) {

  try {
    currentVmeBoard_->unmaskedWrite(fecHalItem_[FECHALRET_FIFO_W], (uint32_t)fiforetValue);

#ifdef FECVMERINGDEVICE_DEBUG
    std::cout << "DEBUG : writing value 0x" << std::hex << fiforetValue << " to fifo return" << std::endl ;
#endif
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
}

/** Return a word from the FIFO transmit
 * \param return a word from the FIFO transmit
 */
tscType32 FecVmeRingDevice::getFifoTransmit( ) throw ( FecExceptionHandler ) {

  uint32_t fifotra_value;

  try {
    currentVmeBoard_->unmaskedRead(fecHalItem_[FECHALTRA_FIFO_R], (haltype *)&fifotra_value);

#ifdef FECVMERINGDEVICE_DEBUG
    std::cout << "Value 0x" << std::hex << fifotra_value << " read from fifo transmit" << std::endl ;
#endif
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return (tscType32)fifotra_value;
}

/** set a word in the FIFO transmit
 * \param fifotra_value - word to be set
 */
void FecVmeRingDevice::setFifoTransmit ( tscType32 fifotraValue ) throw ( FecExceptionHandler ) {

#ifdef FECVMERINGDEVICE_DEBUG
  std::cout << "DEBUG : writing value 0x" << std::hex << fifotraValue << " to fifo transmit" << std::endl ;
#endif

  try {
    currentVmeBoard_->unmaskedWrite(fecHalItem_[FECHALTRA_FIFO_W], (uint32_t)fifotraValue);
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
}


/** set an array of words into the FIFO transmit
 * \param fifotra_value - word to be set
 * \warning if the block transfer is enabled and the version of the FEC VME firmware is correct and if the HAL version is correct then the block transfer is used
 */
void FecVmeRingDevice::setFifoTransmit ( tscType32 *fifotraValue, int count ) throw ( FecExceptionHandler ) {
  
#ifdef FECVMERINGDEVICE_DEBUG 
  {
    int nn ; 
    std::cout << "DEBUG : writing values:  " << std::endl ;
    for(nn=0;nn<count;nn++) 
      std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << fifotraValue[nn] << std::endl ; 
    std::cout << " to fifo transmit" << std::dec << std::endl ;
  }
#endif

  try {
    
    switch (getFifoAccessMode())  { 
    case FECNOBLT : 
      FecRingDevice::setFifoTransmit (fifotraValue,count) ;
      break ; 
    case FECDOBLT :      
      //std::cout << "Transmit in block mode " << count*sizeof(tscType32) << " bytes into the " << fecHalItem_[FECHALTRA_FIFO_W_BL] << " HAL item" << std::endl ;
      currentVmeBoard_->writeBlock(fecHalItem_[FECHALTRA_FIFO_W_BL],
				   (uint32_t)(count*sizeof(tscType32)),
				   (char*) fifotraValue, HAL::HAL_NO_VERIFY, 
				   HAL::HAL_NO_INCREMENT);
      break ;
    default:   
      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_FECPARAMETERNOTMANAGED,
					 "unsupported fifo access mode",
					 FATALERRORCODE,
					 buildFecRingKey(getFecSlot(),getRingSlot()),
					 "mode", getFifoAccessMode()) ;
    }
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  pthread_mutex_lock(&mutexSetFifoTransmitCounter_) ;
  setFifoTransmitCounter_ ++;
  setFifoTransmitSent_ += count*sizeof(tscType32);    
  pthread_mutex_unlock(&mutexSetFifoTransmitCounter_) ;

}

//w.b. modif
/** set an array of words into the FIFO receive
 * \param fiforecValue - array of words to be set
 * \warning if the block transfer is enabled and the version of the FEC VME firmware is correct and if the HAL version is correct then the block transfer is used
 */
void FecVmeRingDevice::setFifoReceive ( tscType32 *fiforecValue, int count ) throw ( FecExceptionHandler ) {
  
#ifdef FECVMERINGDEVICE_DEBUG 
  {
    int nn ; 
    std::cout << "DEBUG : writing values:  " << std::endl ;
    for(nn=0;nn<count;nn++) 
      std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << fiforecValue[nn] << std::endl ; 
    std::cout << " to receive fifo" << std::dec << std::endl ;
  }
#endif

  try {
    
    switch (getFifoAccessMode())  { 
    case FECNOBLT : 
      FecRingDevice::setFifoReceive (fiforecValue,count) ;
      break ; 
    case FECDOBLT : {
      //       std::cout << "Set the receive FIFO in block mode " << count*sizeof(tscType32) << " bytes into the " << fecHalItem_[FECHALREC_FIFO_W_BL] << " HAL item" << std::endl ;
      //       currentVmeBoard_->writeBlock(fecHalItem_[FECHALREC_FIFO_W_BL],
      //  				   (uint32_t)(count*sizeof(tscType32)),
      //  				   (char*)fiforecValue, HAL::HAL_NO_VERIFY, 
      //  				   HAL::HAL_NO_INCREMENT);

      // w.b.
      // bug in firmware, VME BLT write to receive fifo does 
      // not work above 512 D32 words, so we cut it into 512 chunks...
      
      int rest = count ; 
      tscType32 *ptr = fiforecValue;  
      
      while (rest>0) { 
	int nwords = (rest<512) ? rest : 512 ;
	 
	currentVmeBoard_->writeBlock(fecHalItem_[FECHALREC_FIFO_W_BL],
				     (uint32_t)(nwords*sizeof(tscType32)),
				     (char*) ptr, HAL::HAL_NO_VERIFY, 
				     HAL::HAL_NO_INCREMENT);
	rest -= nwords ; 
	ptr += nwords ; 
      }
    }
      break ;
    default:     
      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_FECPARAMETERNOTMANAGED,
					 "unsupported fifo access mode",
					 FATALERRORCODE,
					 buildFecRingKey(getFecSlot(),getRingSlot()),
					 "mode", getFifoAccessMode()) ;
    }
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  catch (FecExceptionHandler &e) {

    throw e ;
  }
  catch (...) {  
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( CODECONSISTENCYERROR,
					    "Exception unknown raised",
					    ERRORCODE,
					    buildFecRingKey(getFecSlot(),getRingSlot())) ;
  }
}

//wbmodif
/** get a array of words from the receive fifo 
 * \param fiforec_value - array word to be retrieved
 * \warning if the block transfer is enable and the version of the FEC VME firmware is correct and if the HAL version is correct then the block transfer is used
 */
tscType32* FecVmeRingDevice::getFifoTransmit ( tscType32 *fifotraValue, int count ) throw ( FecExceptionHandler ) {

  try {
    switch (getFifoAccessMode())  { 
    case FECNOBLT : 
      FecRingDevice::getFifoTransmit(fifotraValue,count) ;
      break ; 
    case FECDOBLT :  
      //std::cout << "Set the transmit FIFO in block mode " << count*sizeof(tscType32) << " bytes into the " << fecHalItem_[FECHALTRA_FIFO_W_BL] << " HAL item" << std::endl ;
      currentVmeBoard_->readBlock(fecHalItem_[FECHALTRA_FIFO_R_BL],
				  (uint32_t)(count*sizeof(tscType32)),
				  (char*) fifotraValue,  
				  HAL::HAL_NO_INCREMENT);
      break ;
    default:   
      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_FECPARAMETERNOTMANAGED,
					 "unsupported fifo access mode",
					 FATALERRORCODE,
					 buildFecRingKey(getFecSlot(),getRingSlot()),
					 "mode", getFifoAccessMode()) ;
    }
    
#ifdef FECVMERINGDEVICE_DEBUG 
    {
      int nn ; 
      std::cout << "FecVmeRingDevice::getFifoTransmit: DEBUG : reading values:  " << std::endl ;
      for(nn=0;nn<count;nn++) 
	std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << fifotraValue[nn] << std::endl ; 
      std::cout << " from transmit fifo" << std::dec << std::endl ;
    }
#endif    
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  catch (FecExceptionHandler &e) {

    throw e ;
  }
  catch (...) { 
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( CODECONSISTENCYERROR,
					    "Exception unknown raised",
					    ERRORCODE,
					    buildFecRingKey(getFecSlot(),getRingSlot())) ;
  } 
  
  return fifotraValue ; 
}




/** get a array of words from the receive fifo 
 * \param fiforec_value - array word to be retrieved
 * \warning if the block transfer is enable and the version of the FEC VME firmware is correct and if the HAL version is correct then the block transfer is used
 */
tscType32* FecVmeRingDevice::getFifoReceive ( tscType32 *fiforecValue, int count ) throw ( FecExceptionHandler ) {

  try {
    switch (getFifoAccessMode())  { 
    case FECNOBLT : 
      FecRingDevice::getFifoReceive(fiforecValue,count) ;
      break ; 
    case FECDOBLT : 
      //std::cout << "------------------------> Receive in block mode " << count*sizeof(tscType32) << " bytes into the " << fecHalItem_[FECHALREC_FIFO_W_BL] << " HAL item" << std::endl ;
      currentVmeBoard_->readBlock(fecHalItem_[FECHALREC_FIFO_R_BL],
 				  (uint32_t)(count*sizeof(tscType32)),
 				  (char*) fiforecValue,  
 				  HAL::HAL_NO_INCREMENT);
      break ;
    default:   
      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_FECPARAMETERNOTMANAGED,
					 "unsupported fifo access mode",
					 FATALERRORCODE,
					 buildFecRingKey(getFecSlot(),getRingSlot()),
					 "mode", getFifoAccessMode()) ;
    }
    
#ifdef FECVMERINGDEVICE_DEBUG 
    {
      int nn ; 
      std::cout << "FecVmeRingDevice::setFifoReceive: DEBUG : reading values:  " << std::endl ;
      for(nn=0;nn<count;nn++) 
	std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << fiforecValue[nn] << std::endl ; 
      std::cout << " from receive fifo" << std::dec << std::endl ;
    }
#endif    
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  catch (FecExceptionHandler &e) {

    throw e ;
  }
  catch (...) { 
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( CODECONSISTENCYERROR,
					    "Exception unknown raised",
					    ERRORCODE,
					    buildFecRingKey(getFecSlot(),getRingSlot())) ;
  } 
  
  return fiforecValue ; 
}

/******************************************************
		HARD RESET
******************************************************/	

/** Hard reset of the FEC board or the ring board
 * \warning For the moment the FEC ring device is not implemented and cannot be implemented (status from 23/04/04).
 * \warning A FEC board reset will be implemented. The problem with this reset is that if you reset the board all the rings
 * and not this specific ring will be reseted. 
 * \warning Due to the fact that FEC board reset is not yet implemented, we use for the moment the crate reset, static method
 * from this class
 * \warning In the future if the hard FEC ring reset is not implemented then this method will be moved to the FecAccess
 * the same for the crate and fecHardReset will dispaeared.
 * \exception FecExceptionHandler
 * \warning Only the mFECs
 * 
 */
void FecVmeRingDevice::fecHardReset ( ) throw ( FecExceptionHandler ) {

  // static method
  uFecsReload() ;

  // Re-initialise all tables used in FecRingDevice
  FecRingDevice::initAfterHardReset() ;
}

/******************************************************
		IRQ enable / disable
******************************************************/	

/** IRQ address, enable and disable the IRQ
 * ---------------------------------------------------------------------------------
 * Christer
 * INT_MASK6	2f 1 0037FFE7 ffffffff 1 1 
 * INT_LEVEL6	2f 1 0037FFE3 ffffffff 1 1
 *
 * INT_MASK bit[7] = FEC7; INT_MASK[0] = FEC0; "1" DISABLES FEC INT
 * INT_LEVEL bit[3] = IRQ ENABLE
 * INT_LEVEL bit[2:0] = IRQ LEVEL; 001 = IRQ[1]; 111 = IRQ[7]
 * INTERRUPT VECTOR[7:0] = {SLOT_NUMBER[4:0], FEC_NUMBER[2:0]}; 7'h32 = SLOT6 FEC_2
 * ---------------------------------------------------------------------------------
 * INT_MASK = 0000 0000
 * 
 * \warning This method is not used until the IRQ will be managed by the VME FEC / software
 */
void FecVmeRingDevice::setIRQ ( bool enable, tscType8 level ) throw ( FecExceptionHandler ) {

  /* empty function: do nothing , USB is not interrupt driven, no risc of interrupt overflood */

  return ;

  char maskItem[12];
  char levelItem[12];
  uint32_t maskValue, irqLevel ;

  try {

    snprintf(maskItem, 12, "INT_MASK%d", getFecSlot());
    snprintf(levelItem, 12, "INT_LEVEL%d", getFecSlot());

    for (int i = 1 ; i < 8 ; i ++) {

      maskValue = 1 << i ;

      currentVmeBoard_->unmaskedWrite(maskItem, maskValue) ;
      currentVmeBoard_->unmaskedRead(maskItem, (haltype *)&maskValue) ;
      std::cout << std::dec << i << ": MASK = 0x" << std::hex << (maskValue & 0xFF) << " and must be " << (1 << i) << std::endl ;
    }

    for (level = 1 ; level < 8 ; level ++) {

      if (enable) {
	  
	// For the board, enable IRQ and the mask can enable/disable each ring
	irqLevel = 0x8 ;
      }
      else irqLevel = 0 ;

      irqLevel |= level ;
      currentVmeBoard_->unmaskedWrite(levelItem, irqLevel);
      currentVmeBoard_->unmaskedRead(levelItem, (haltype *)&irqLevel);

      std::cout << std::dec << level << ": LEVEL = " << std::hex << (irqLevel & 0xF) << " and must be " << std::hex << (0x8 | level) << std::endl ;
    }

    getchar() ;
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
} 

/******************************************************
		BOARD RESET & CONFIGURATION FUNCTIONS
******************************************************/

/** Create the bus adapter depending of which one was chosen
 * This method is only used for the creation of the busAdapter depending of which PCI to VME board you are using
 * \param adapterSlot - where the adapter is
 * \param strBusAdapter - string with the bus adapter name. A static attributs exists FecVmeRingDevice::STR_BUSADAPTER_NAME together with an enumeration enumBusAdapter
 */
HAL::VMEBusAdapterInterface *FecVmeRingDevice::createBusAdapter ( uint32_t adapterSlot, std::string strBusAdapter ) throw ( HAL::BusAdapterException, FecExceptionHandler ) {

  HAL::VMEBusAdapterInterface *busAdapter = NULL ;

  if (strBusAdapter == FecVmeRingDevice::STR_BUSADAPTER_NAME[CAENPCI] ) {
#if defined(BUSVMECAENPCI)

    busAdapter = new HAL::CAENLinuxBusAdapter(HAL::CAENLinuxBusAdapter::V2718,adapterSlot,0,HAL::CAENLinuxBusAdapter::A3818) ;
#else
    RAISEFECEXCEPTIONHANDLER ( CODECONSISTENCYERROR,
			       "Support for CAEN PCI is not compiled and cannot be used",
			       FATALERRORCODE) ;
#endif
  }
  else if (strBusAdapter == FecVmeRingDevice::STR_BUSADAPTER_NAME[CAENUSB] ) {
#if defined (BUSVMECAENUSB)
    busAdapter = new HAL::CAENLinuxBusAdapter(HAL::CAENLinuxBusAdapter::V1718,adapterSlot) ;
#else
    RAISEFECEXCEPTIONHANDLER ( CODECONSISTENCYERROR,
			       "Support for CAEN USB is not compiled and cannot be used",
			       FATALERRORCODE) ;
#endif
  }
  else if (strBusAdapter == FecVmeRingDevice::STR_BUSADAPTER_NAME[SBS]) {

#if defined (BUSVMESBS)
    busAdapter = new HAL::SBS620x86LinuxBusAdapter(adapterSlot);
#else
    RAISEFECEXCEPTIONHANDLER ( CODECONSISTENCYERROR,
			       "Support for SBS is not compiled and cannot be used",
			       FATALERRORCODE) ;
#endif
  }
  else {
    RAISEFECEXCEPTIONHANDLER ( CODECONSISTENCYERROR,
			       "Unknown bus support: " + strBusAdapter,
			       FATALERRORCODE ) ;
  }

  return (busAdapter) ;
}

/** Plug and Play function for one VME crate
 * \param adapterSlot - which interface (daisy chain is used)
 * \param addresstableFile -
 * \param maptable -
 * \param strBusAdapter - which kind of bus adapter
 * \param listOfVMEDevice - returns the list of devices detected
 */
HAL::VME64xCrate *FecVmeRingDevice::configurePlugAndPlay( uint32_t adapterSlot, std::string addresstablefile, std::string maptable, std::string strBusAdapter, hashMapVMEDevice &hashMapVMEDevice )
  throw (HAL::BusAdapterException,
	 HAL::NoSuchFileException,
	 HAL::IllegalValueException,
	 HAL::XMLProcessingException,
	 HAL::HardwareProblemException,
	 HAL::UnsupportedException ) {

#ifdef FECVMERINGDEVICE_DEBUG
  std::cout << "configurePlugAndPlay with file " << std::endl
	    << "\t" << maptable << " for the index of the registers" << std::endl
	    << "\t" << addresstablefile << " for the FEC hardware ID" << std::endl ;
#endif

  //time_t time0, time1 ;
  //time0 = time(NULL) ;

  // Mutex to avoid mutithread access
  pthread_mutex_lock(&mutexBusAdapter_) ;

  if (busAdapter_.find(adapterSlot) == busAdapter_.end()) {

    busAdapter_[adapterSlot] = createBusAdapter ( adapterSlot, strBusAdapter ) ;
  }
  // unlock the mutex
  pthread_mutex_unlock(&mutexBusAdapter_) ;

  std::cerr << "FecVmeRingDevice::configurePlugAndPlay (after mutex): returned from createBusAdapter and busAdapter_ is " << busAdapter_[adapterSlot] << " (should not be null)" << std::endl;

  if (busAdapter_[adapterSlot] == NULL) {

    RAISEFECEXCEPTIONHANDLER ( HAL_BUSADAPTEREXCEPTION,
			       "Unable to create the bus adapter for: " + strBusAdapter,
			       FATALERRORCODE ) ;
  }

  if (moduleMapper_ == NULL) moduleMapper_ = new HAL::ASCIIFileModuleMapper(addresstablefile);
  if (addressTableContainer_ == NULL) addressTableContainer_ = new HAL::ASCIIFileAddressTableContainer((*moduleMapper_), maptable);

  HAL::VME64xCrate *vme64xcrate = new HAL::VME64xCrate((*busAdapter_[adapterSlot]), (*addressTableContainer_), (*moduleMapper_));

  //time1 = time(NULL) ;
  //std::cout << "vme64xcrate creation takes " << (time1-time0) << " seconds" << std::endl ;

  // detection of the numbering schema done once
  bool numberSchema = true ;

  // Check if the card at the given slot is FEC or not
  for(uint32_t slotId = 1 ; slotId < MAX_NUMBER_OF_SLOTS ; slotId++ ) {
    
    HAL::VMESlot *vmeSlot = vme64xcrate->getVMESlot(slotId);

    if(vmeSlot != 0) {
      
      std::string typeId = vmeSlot->getTypeId();
      
#ifdef FECVMERINGDEVICE_DEBUG
      std::cout << "Slot = " << std::dec << slotId <<" typeId = " << typeId << std::endl ;
#endif

      // Check if it is a FEC-CCS board
      if ( typeId == "FEC-CCS") {
	
	std::string serNum = vmeSlot->getSerialNumber();
	
#ifdef FECVMERINGDEVICE_DEBUG
	std::cout << "Serial number= " << serNum << std:: endl;
#endif
	
	HAL::VMEAddressTable addressTable = addressTableContainer_->getVMETableFromSerialNumber( serNum );

	if (numberSchema) {
	  // detect the numbering schema used
	  autoNumberingSchema (addressTable) ;
	  numberSchema = false ;
	}

#ifdef FECVMERINGDEVICE_DEBUG
	std::cout << "addressTable = 0x" << std::hex << (uint32_t)&addressTable << std::endl;
#endif

	HAL::VME64xDevice *vmeDevice = (HAL::VME64xDevice *)vmeSlot->getVMEDevice() ;

	if (vmeDevice != NULL) hashMapVMEDevice[slotId] = vmeDevice ;
      }
    }
  }

  //FecVmeRingDevice::plugnplayUsed_ = true ;
  //std::cout << "" << std::endl ;
  //std::cout << "End of the configuration plug and play" << std::endl ;

  return vme64xcrate;
}


/** Configure a FEC based on the address given by the baseAddress parameter
 * \param adapterSlot - where the adapter is
 * \param addresstablefile - the file for the items addressing
 * \param baseaddress - base address of the board
 * \param fecSlot - FEC slot
 * \param strBusAdapter - PCI to VME link used see the method createBusAdapter
 */
HAL::VME64xDevice * FecVmeRingDevice::configureHardBaseAddress( uint32_t adapterSlot, std::string addresstablefile, uint32_t baseaddress, uint32_t fecSlot, std::string strBusAdapter )
  throw (HAL::BusAdapterException,
	 HAL::NoSuchFileException,
	 HAL::IllegalValueException,
	 HAL::UnsupportedException,
	 FecExceptionHandler) {

  // Mutex to avoid mutithread access
  pthread_mutex_lock(&mutexBusAdapter_) ;

  if (busAdapter_.find(adapterSlot) == busAdapter_.end()) {

    busAdapter_[adapterSlot] = createBusAdapter ( adapterSlot, strBusAdapter ) ;
  }

  std::cerr << "FecVmeRingDevice::configureHardBaseAddress (after mutex): returned from createBusAdapter and busAdapter_ is " << busAdapter_[adapterSlot] << " (should not be null)" << std::endl;

  if (busAdapter_[adapterSlot] == NULL) {

    RAISEFECEXCEPTIONHANDLER ( HAL_BUSADAPTEREXCEPTION,
			       "Unable to create the bus adapter for: " + strBusAdapter,
			       FATALERRORCODE ) ;
  }


  if (addressTableReader_ == NULL) addressTableReader_ = new HAL::VMEAddressTableASCIIReader(addresstablefile);
  if (addressTable_ == NULL) addressTable_ = new HAL::VMEAddressTable("FEC Address Table", (*addressTableReader_));

  HAL::VME64xDevice * vmeBoard = (HAL::VME64xDevice *)new HAL::VMEDevice((*addressTable_), (*busAdapter_[adapterSlot]), baseaddress);

  // detect the numbering schema used
  autoNumberingSchema (addresstablefile) ;

  // Check if the board is correct and it is a FEC or not
  try {
    if (detectVmeBoard (vmeBoard, fecSlot)) {

      delete vmeBoard ; vmeBoard = NULL ;
      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_VMEACCESSPROBLEM,
					 "no board is connected on this slot",
					 FATALERRORCODE,
					 setFecSlotKey(fecSlot),
					 "Adapter slot", adapterSlot) ;
    }
  }
  catch (FecExceptionHandler &e) {
  // unlock the mutex
    pthread_mutex_unlock(&mutexBusAdapter_) ;

    delete vmeBoard ; vmeBoard = NULL ;
    throw e ;
  }

  //FecVmeRingDevice::plugnplayUsed_ = false ;  

  //time1 = time(NULL) ;
  //std::cout << "Creation of one board " << (time1-time0) << " seconds" << std::endl ;

  // unlock the mutex
  pthread_mutex_unlock(&mutexBusAdapter_) ;


  return vmeBoard;
}

/** Detect which is the numbering schema used and set the variable 
 * FEC ring can goes from 0 to 7 from top to bottom or from 1 to 8 from bottom to top
 */
void FecVmeRingDevice::autoNumberingSchema ( HAL::VMEAddressTable addressTable ) {

  std::string item("CONTROL0_8") ;

  if (addressTable.exists(item)) {
    MINVMEFECRING = 1 ;
    MAXVMEFECRING = 8 ;
  }
  else {
    MINVMEFECRING = 0 ;
    MAXVMEFECRING = 7 ;
  }
 
#ifdef FECVMERINGDEVICE_DEBUG
  std::cout << "FEC ring goes from " << MINVMEFECRING << " to " << MAXVMEFECRING << std::endl ;
#endif
}

/** Detect which is the numbering schema used and set the variable 
 * FEC ring can goes from 0 to 7 from top to bottom or from 1 to 8 from bottom to top
 */
void FecVmeRingDevice::autoNumberingSchema ( std::string addresstablefile ) {

  std::string item("CONTROL0_1_8") ;
  HAL::VMEAddressTableASCIIReader addressTableReader(addresstablefile) ;
  HAL::VMEAddressTable addressTable("FEC Address Table", addressTableReader);

  autoNumberingSchema ( addressTable ) ;
}

/** Detect which is the numbering schema used and set the variable
 * FEC ring can goes from 0 to 7 from top to bottom or from 1 to 8 from bottom to top
 * \warning for VME64X
 */
void FecVmeRingDevice::autoNumberingSchema ( const HAL::AddressTableInterface& addressTableInterface ) {
                                                                                                                                                
  std::string item("CONTROL0_8") ;
                                                                                                                                                
  if (addressTableInterface.exists(item)) {
    MINVMEFECRING = 1 ;
    MAXVMEFECRING = 8 ;
  }
  else {
    MINVMEFECRING = 0 ;
    MAXVMEFECRING = 7 ;
  }
                                                                                                                                                
#ifdef FECVMERINGDEVICE_DEBUG
  std::cout << "FEC ring goes from " << MINVMEFECRING << " to " << MAXVMEFECRING << std::endl ;
#endif
}

/* Reads a value from a FEC register, on ring 0. This method is actually used
   to perform a crate probing, until I can access to configuration registers of the
   board itself and not only the rings registers.
   The first mezzanine must be always plugged at the first place from the top of the FEC. This is mandatory for the load of the firmware
*/
tscType16 FecVmeRingDevice::detectVmeBoard(HAL::VME64xDevice *vmedevice, tscType8 fecSlot) throw ( FecExceptionHandler ) {

  uint32_t confValue;
  
#ifdef FECVMERINGDEVICE_DEBUG
  std::cout << "Now working in board detection method for slot " << std::dec << (int)fecSlot << " on the item " << "VME_VERSION_FW" << std::endl;
#endif

  char item[] = "VME_VERSION_FW" ;
  //   char item[80] ;
  //   strcpy (item, FECHALITEM[FECHALVERSION_SRC] ) ;
  //   if (MINVMEFECRING == 1)
  //     item[FECHALCHARITEM[FECHALVERSION_SRC]] = MAXVMEFECRING + NUMBERASCIIDIFF ;
  //   else
  //     item[FECHALCHARITEM[FECHALVERSION_SRC]] = MINVMEFECRING + NUMBERASCIIDIFF ;

  try {
    vmedevice->unmaskedRead(item, (haltype *)&confValue);

  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(fecSlot) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(fecSlot) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(fecSlot) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(fecSlot) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(fecSlot) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(fecSlot) ) ;
    //e.what()) ;
  }
  
#ifdef FECVMERINGDEVICE_DEBUG
  std::cout << "In board detection : value read is : " << std::hex << confValue << std::endl << std::endl;
#endif
  
  return 0x0000;
}

/** Delete all the statics value allowed for VME 64x or in automatic detection
 * Never call this method if you are multithreaded, the busAdapter_ is shared between the threads
 */
void FecVmeRingDevice::closeFecVmeAccess ( ) throw (HAL::BusAdapterException) {

  if (busAdapter_.size() != 0) {
    for (Sgi::hash_map<uint32_t, HAL::VMEBusAdapterInterface *>::iterator vme = busAdapter_.begin() ; vme != busAdapter_.end() ; vme ++) {
      if (vme->second != NULL) delete vme->second ;
    }
    busAdapter_.clear() ;
  }
  if (moduleMapper_ != NULL) {
    delete moduleMapper_;
    moduleMapper_ = NULL ;
  }
  if (addressTableContainer_ != NULL) {
    delete addressTableContainer_;
    addressTableContainer_ = NULL ;
  }
  if (addressTableReader_ != NULL) {
    delete addressTableReader_;
    addressTableReader_ = NULL ;
  }
  if (addressTable_ != NULL) {
    delete addressTable_;
    addressTable_ = NULL ;
  }
}

/** Hard reset of the VME crate
 */
void FecVmeRingDevice::crateReset ( uint32_t adapterSlot ) throw ( FecExceptionHandler ) {
  pthread_mutex_lock(&mutexCrateReset_);
  if (crateResetDone) {
    std::cerr << "Crate reset for adapterSlot "<<adapterSlot<< " already done !" << std::endl;
    pthread_mutex_unlock(&mutexCrateReset_);
    return;
  }
  crateResetDone = true;

  std::cerr << "Doing crate reset for adapterSlot "<<adapterSlot << std::endl;
  try {
    if (busAdapter_.find(adapterSlot) != busAdapter_.end()) busAdapter_[adapterSlot]->resetBus();
    else {
      pthread_mutex_unlock(&mutexCrateReset_);
      RAISEFECEXCEPTIONHANDLER ( TSCFEC_VMEACCESSPROBLEM,
				    "bus adapter is not created on adapter slot " + toString(adapterSlot),
				    FATALERRORCODE ) ;
    }

#ifdef LOGRELOADFIRMARE
    std::ofstream fichier (LOGRELOADFIRMARE, std::ios_base::app ) ;
    if (!fichier) 
      std::cerr << "Online Running> warning cannot open the file " << LOGRELOADFIRMARE << " to log the reset of the crate" << std::endl ;
    else {
      time_t coucou = time(NULL) ;
      fichier << ctime(&coucou) << " crate reset performed" << std::endl ;
      if (!fichier) std::cerr << "Online Running> warning cannot write in file " << LOGRELOADFIRMARE << " to log the reset of the crate" << std::endl ;
      else fichier.close() ;
    }
#endif
  }
  catch ( HAL::BusAdapterException &e ) {
    pthread_mutex_unlock(&mutexCrateReset_);
    RAISEFECEXCEPTIONHANDLER ( TSCFEC_VMEACCESSPROBLEM,
			       "Hardware problem access (HAL::BusAdapterException): " + (std::string)e.what() + " on bus adapter slot " + toString(adapterSlot),
			       FATALERRORCODE ) ;
    // e.what()) ;
  }
  catch (FecExceptionHandler &e) {
    pthread_mutex_unlock(&mutexCrateReset_);
    throw e ;
  }
  pthread_mutex_unlock(&mutexCrateReset_);
}

/******************************************************
           VME64x CSR/CR space operations
******************************************************/

/** Read an hardware ID from a VME board
 * \param slot of the VME board
 */
std::string FecVmeRingDevice::getSerialNumber ( uint32_t slot, uint32_t adapterSlot ) throw ( FecExceptionHandler ) {

  std::string serialNumber ;

  try {
    if (busAdapter_.find(adapterSlot) != busAdapter_.end()) {

      //std::cout << "Before the config handler (adapterSlot = " << adapterSlot << ")" << std::endl ;

      HAL::VMEConfigurationSpaceHandler configHandler ( *busAdapter_[adapterSlot] ) ;
      serialNumber = configHandler.readSerialNumber(slot) ;

#ifdef FECVMERINGDEVICE_DEBUG
      std::cout << "Serial number on FEC slot " << std::dec << slot << " = " << serialNumber << std::endl ;
#endif
    }
    else RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_VMEACCESSPROBLEM,
					    "bus adapter is not created",
					    ERRORCODE,
					    setFecSlotKey(slot),
					    "Adapter slot", adapterSlot) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {
    //e.what() ;
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(slot) ) ;
  }

  return serialNumber ;
}

/**  \brief return serial number 
 */
std::string FecVmeRingDevice::getSerialNumber () {
  
  std::string serialNumber ;
  serialNumber = currentVmeBoard_->getSerialNumber() ;
  
#ifdef FECVMERINGDEVICE_DEBUG
  std::cout << "Serial number " << serialNumber << std::endl ;
#endif

  return serialNumber ;
}

/**  \brief return trigger version number (firmware)
 */
tscType32 FecVmeRingDevice::getTriggerVersion() throw ( FecExceptionHandler ) {
  uint32_t triggerVersion, value;
  try {

    currentVmeBoard_->read("TRIG_ID",(haltype *)&value);

    //TRIG_ID<7:4> is the base version
    //TRIG_ID<3:0> is the subversion 
#define MASKTRIGGERVERSION 0xFF

#ifdef FECVMERINGDEVICE_DEBUG
    std::cout << "Value is = " << std::dec << value << std::endl ;
    std::cout << "Value is = 0x" << std::hex << value << std::endl ;
    std::cout << "Major = " << std::hex << (value & 0xF0) << std::endl ;
    std::cout << "Minor = " << std::hex << (value & 0x0F) << std::endl ;
    std::cout << "Version = " << std::hex << (value & MASKTRIGGERVERSION) << std::endl ;
#endif

    triggerVersion = (value & MASKTRIGGERVERSION) ;
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

#ifdef FECVMERINGDEVICE_DEBUG
  std::cout << "Trigger slave version  " << triggerVersion << std::endl ;
#endif
  return (tscType32) triggerVersion;
}

/**  \brief return VME slave version number
 */
tscType32 FecVmeRingDevice::getVmeVersion() throw ( FecExceptionHandler ) {
  uint32_t VmeVersion;
  try {
    currentVmeBoard_->read("VME_VERSION_FW",(haltype *)&VmeVersion);
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  catch (...) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( CODECONSISTENCYERROR,
					    "Exception unknown raised",
					    ERRORCODE,
					    buildFecRingKey(getFecSlot(),getRingSlot())) ;
  }

#ifdef FECVMERINGDEVICE_DEBUG
  std::cout << "VME slave version  " << VmeVersion << std::endl ;
#endif
  return (tscType32) VmeVersion;
}

/**  \brief Reload all the uFECs on the board. Sleep about 1 sec after that.
 * \param value - 1 for mFECs (by default), 2 for trigger FPGA, 4 for VME FPGA
 */
void FecVmeRingDevice::uFecsReload( tscType8 value ) throw ( FecExceptionHandler ) {

  try {
    currentVmeBoard_->write("RELOAD_FW",value);
    usleep(500000) ;

    bool error = true ; int timeout = 0 ;
    while (error && (timeout <  10)) {
      try {
	getFecRingSR0() ;
	error = false ;
      }
      catch (FecExceptionHandler &e) { 
	//#ifdef FECVMERINGDEVICE_DEBUG
	std::cout << "Error in getting the SR0 (timeout = " << std::dec << timeout << ") on FEC " << (int)getFecSlot() << "." << (int)getRingSlot() << std::endl ;
	//#endif
	usleep(500000) ;
	timeout ++ ;
      }
    }

#ifdef LOGRELOADFIRMARE
    std::ofstream fichier (LOGRELOADFIRMARE, std::ios_base::app ) ;
    if (!fichier) 
      std::cerr << "Online Running> warning cannot open the file " << LOGRELOADFIRMARE << " to log the reload of firmware" << std::endl ;
    else {
      time_t coucou = time(NULL) ;
      fichier << ctime(&coucou) << " reload of firmware was done on ring " << (int)getFecSlot() << "." << (int)getRingSlot() << std::endl ;
      if (!fichier) std::cerr << "Online Running> warning cannot write in file " << LOGRELOADFIRMARE << " to log the reload of firmware" << std::endl ;
      else fichier.close() ;
    }
#endif

    //getFecRingSR0() ;
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
}


unsigned long FecVmeRingDevice::getSr0Counter() {
  unsigned long n;
  pthread_mutex_lock(&mutexGetRingSr0Counter_) ;
  n=getRingSr0Counter_;    
  pthread_mutex_unlock(&mutexGetRingSr0Counter_) ;
  return n;
}

unsigned long FecVmeRingDevice::getTraFifoSent() {
  unsigned long n;
  pthread_mutex_lock(&mutexSetFifoTransmitCounter_) ;
  n=setFifoTransmitSent_;    
  pthread_mutex_unlock(&mutexSetFifoTransmitCounter_) ;
  return n;
}

unsigned long FecVmeRingDevice::getTraFifoCounter() {
  unsigned long n;
  pthread_mutex_lock(&mutexSetFifoTransmitCounter_) ;
  n=setFifoTransmitCounter_;    
  pthread_mutex_unlock(&mutexSetFifoTransmitCounter_) ;
  return n;
}

/** Initialise the static variable for the base addresses
 */
const uint32_t FecVmeRingDevice::VMEFECBASEADDRESSES[MAX_NUMBER_OF_SLOTS] =
  {0, 0x08000000, 0x10000000, 0x18000000, 0x20000000, 0x28000000, 0x30000000, 
   0x38000000, 0x40000000, 0x48000000, 0x50000000, 0x58000000, 0x60000000, 
   0x68000000, 0x70000000, 0x78000000, 0x80000000, 0x88000000, 0x90000000, 
   0x98000000, 0xA0000000, 0xA8000000 } ;

/** item string for the ring 0, the value _0 must be change with the ring number
 */
const char FecVmeRingDevice::FECHALITEM[][32]={"CONTROL0_0", "CONTROL1_0", "STATUS0_0", "STATUS1_0", "VERSION_SRC_0", "TRA_FIFO_0_R", "RET_FIFO_0_R", "REC_FIFO_0_R", "TRA_FIFO_0_W", "RET_FIFO_0_W", "REC_FIFO_0_W",
					       "TRA_FIFO_0_R_BL", "RET_FIFO_0_R_BL", "REC_FIFO_0_R_BL", "TRA_FIFO_0_W_BL", "RET_FIFO_0_W_BL", "REC_FIFO_0_W_BL"} ;

/** Which charater must be changed in the string
 */
const int FecVmeRingDevice::FECHALCHARITEM[]={9, 9, 8, 8, 12, 9, 9, 9, 9, 9, 9,9, 9, 9, 9, 9, 9} ;


/** Intialise the static variable for the minimum ring and the maximum ring
 */
uint32_t FecVmeRingDevice::MINVMEFECRING = 0 ;
uint32_t FecVmeRingDevice::MAXVMEFECRING = 7 ;
int plugnplayUsed_ = false ;

/** Bus adapter name
 */
const std::string FecVmeRingDevice::STR_BUSADAPTER_NAME[3] = {"SBS620x86LinuxBusAdapter", "CAEN2718LinuxPCIBusAdapter", "CAEN1718LinuxUSBBusAdapter"};
