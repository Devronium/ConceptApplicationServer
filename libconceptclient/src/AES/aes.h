/* AES - Advanced Encryption Standard
 *
 * source version 0.9, Nov 2, 2000
 *
 * Copyright (C) 2000 Chris Lomont
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the author be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *   claim that you wrote the original software. If you use this software
 *   in a product, an acknowledgment in the product documentation would be
 *   appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *   misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Chris Lomont
 * clomont@math.purdue.edu
 *
 * The AES Standard is maintained by NIST
 * http://csrc.nist.gov/encryption/aes/
 *
 * This legalese is patterened after the zlib compression library
 */

// headed to implement Advanced Encryption Standard - Rijndael2
#ifndef _AES_H
#define _AES_H

#define AES_int32     int
#define AES_uint32    unsigned AES_int32

/* USAGE:
 *      1. Call CreateAESTables to check or create data tables
 *      2. Create a AES class (or more as necessary)
 *      3. Call class method SetParameters
 *      4. To Encrypt, call method StartEncryption with the key, and then
 *              call method Encrypt as many times as needed to encrypt a block of data each pass
 *      5. To Decrypt, call method StartDecryption with the key, and then
 *              call method Decrypt as many times as needed to encrypt a block of data each pass
 */

// todo - replace all types with u1byte, u4byte, etc

class AES
{
private:

    int Nb, Nk;                  // block and key length / 32, should be 4,6,or 8
    int Nr;                      // number of rounds

    unsigned char W[4 * 8 * 15]; // the expanded key

// Key expansion code - makes local copy
    void KeyExpansion(const unsigned char *key);

public:
// block and key size are in bits, legal
// values are 128, 192, and 256 independently
    void SetParameters(int keylength, int blocklength);

    void StartEncryption(const unsigned char *key);
    void Encrypt(AES_uint32 *datain, AES_uint32 *dataout);

    void StartDecryption(unsigned char *key);
    void Decrypt(AES_uint32 *datain, AES_uint32 *dataout);
};         // class AES


// if create = true, this will make all tables
// if create_file = true, this will write out a text file of all tables
// so that can be placed back into the code
// if create = false, this will return true iff tables are valid
bool CreateAESTables(bool create, bool create_file = false);


#endif //  _AES_H
// end - AES.h
