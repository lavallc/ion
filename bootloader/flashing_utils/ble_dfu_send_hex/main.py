import time
import argparse

from dfu.ble_dfu import BleDfu

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Send hex file over-the-air via BLE')
    parser.add_argument('--file', '-f',
                        type=str,
                        required=True,
                        dest='file',
                        help='Filename of Hex file.')
    parser.add_argument('--name', '-n',
                        type=str,
                        required=True,
                        dest='name',
                        help='Advertising name of nrf51822 device.')
    parser.add_argument('--address', '-a',
                        type=str,
                        required=True,
                        dest='address',
                        help='Advertising address of nrf51822 device.')
    
    args = parser.parse_args()
    print 'Sending file {0} to device {1}/{2}'.format(args.file, args.name, args.address.upper())

    ble_dfu = BleDfu(args.name, args.address.upper(), args.file)
    
    # Connect to peer device.
    ble_dfu.scan_and_connect()
    
    # Transmit the hex image to peer device.
    ble_dfu.dfu_send_image()
    
    # wait a second to be able to recieve the disconnect event from peer device.
    time.sleep(1)
    
    # Disconnect from peer device if not done already and clean up. 
    ble_dfu.disconnect()
