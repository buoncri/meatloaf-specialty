// Meatloaf - A Commodore 64/128 multi-device emulator
// https://github.com/idolpx/meatloaf
// Copyright(C) 2020 James Johnston
//
// Meatloaf is free software : you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Meatloaf is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Meatloaf. If not, see <http://www.gnu.org/licenses/>.

//
// This class will include ZoomFloopy/IECHOST functionality
//
// https://github.com/go4retro/ZoomFloppy
// https://luigidifraia.wordpress.com/tag/iechost/
// https://sourceforge.net/projects/opencbm/
//

#include "iec_host.h"

#include "../../include/debug.h"
#include "../../include/pinmap.h"

using namespace Protocol;


bool iecHost::deviceExists(uint8_t deviceID)
{
    bool device_status = false;

    Debug_printf("device [%d] ", deviceID);

    // Get Bus Attention
    protocol.pull(PIN_IEC_ATN);
    protocol.pull(PIN_IEC_CLK_OUT);
    //release(PIN_IEC_DATA_OUT);
    //delayMicroseconds(TIMING_ATN_PREDELAY);

    // Wait for listeners to be ready
    if(protocol.timeoutWait(PIN_IEC_DATA_IN, PULLED) == TIMED_OUT)
    {
        Debug_println(" inactive\nBus empty. Exiting.");
    }
    else
    {
        // Send Listen Command & Device ID
        Debug_printf( "%.2X", (IEC_LISTEN & deviceID));
        send( IEC_LISTEN & deviceID );
        delayMicroseconds(TIMING_Tv);

        if ( protocol.status( PIN_IEC_DATA_IN ) )
        {
            device_status = true;
            Debug_println("active");
        }
        else
        {
            device_status = false;
            Debug_println("inactive");
        }
        delayMicroseconds(TIMING_Tv);

        // Send UnListen
        send( IEC_UNLISTEN );
        delayMicroseconds(TIMING_Tv);
    }

    // Release ATN and Clock
    protocol.release(PIN_IEC_ATN);
    protocol.release(PIN_IEC_CLK_OUT);
    delayMicroseconds(TIMING_Tv);

    return device_status;
}