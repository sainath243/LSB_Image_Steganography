# LSB_Image_Steganography
🕵️‍♂️ Steganography Project in C This is a terminal-based Steganography System built in C that allows secure embedding and extraction of secret messages within .bmp image files using Least Significant Bit (LSB) encoding technique. It enables basic steganographic operations such as encoding text data into an image and decoding hidden information from an image without visibly altering it. The system emphasizes modular design, file handling, bitwise operations, and strict validation to ensure correctness and robustness.

📌 Project Overview Traditional methods of securing information such as encryption draw attention due to obvious transformations in data. Steganography offers a discreet alternative by concealing the very presence of the message. This project implements a digital solution that allows the user to embed or extract text messages into/from .bmp files via a command-line interface. It is designed with persistent file access, clear modular structure, and a flexible file-handling system.

⚙️ Tools and Technologies Used C Programming Language

GCC Compiler

Command-Line Interface (CLI)

24-bit BMP Image Format

Modular Programming Design

📁 File Modules less Copy Edit

├── test_encode.c // Handles command-line arguments and dispatches encode/decode operation

├── encode.c / encode.h // Encoding logic: embeds data into BMP image

├── decode.c / decode.h // Decoding logic: extracts data from stego image

├── types.h // Status codes and enum definitions

├── Makefile // Build automation

├── stego.bmp // Output BMP file with embedded data

├── secret.txt // Input text file to encode

├── output.txt // Output text file after decoding

🧠 Working Principle The system follows the Least Significant Bit (LSB) method to hide and recover messages from BMP files. It manipulates the least significant bit of each image byte to encode or decode binary data, ensuring the visual integrity of the image remains unaffected.

🔧 Features ✅ Encode secret messages into BMP files

✅ Decode hidden messages from stego images

✅ Uses magic string to verify presence of hidden data

✅ Bitwise encoding using LSB manipulation

✅ File validation and error checking

✅ Modular architecture for scalability

✅ Supports any printable character set

✅ CLI arguments for flexibility

🗃️ Command-Line Usage Encoding:

Encoding: ./a.out -e <source_image.bmp> <secret.txt> <stego_image.bmp>
Decoding: ./a.out -d <stego_image.bmp> <output.txt> 

🧪 Input Validation:
✅ Validates .bmp file format (supports only 24-bit BMP)

✅ Validates file pointers for all input/output files

✅ Checks magic string for presence of embedded data

✅ Validates image size is sufficient for data encoding

🛠️ Core Operations 
🔹 Encode Reads secret text file Embeds magic string, file extension, secret size, and content using LSB Outputs a new stego image that visually matches the original

🔹 Decode Reads stego image Extracts and validates magic string Decodes file extension and secret content Writes decoded content to output file

🔐 Security & Limitations Steganography hides presence of data, not content — not a substitute for encryption.

Can only encode limited data depending on image size.

No authentication or encryption layers implemented (future scope).

📈 Future Enhancements 🔒 Integrate with encryption for dual-layer security

🖼️ Support for other image formats (PNG, JPEG)

🧾 GUI version using GTK or Qt

📊 Add image metadata manipulation

👤 Password-protected access

📜 Logging and detailed report generation

📦 Archive multiple files using ZIP + encode

📸 Sample Terminal Output: 
<img width="1366" height="768" alt="3" src="https://github.com/user-attachments/assets/b9dceab3-3511-42f6-b9e0-773fadd68e1c" />

DECODING OPERATION STARTED
Opening image file... Done Verifying magic string... Verified ✅ Extracting file extension... Done Reading secret data... Completed successfully!

✅ Output written to output.txt 
📸 Sample Terminal Output: 
<img width="786" height="383" alt="4" src="https://github.com/user-attachments/assets/8624b9f1-9e40-4333-b836-2e8bd30bae7a" />

👨‍💻 Developed By C.Saianth Aspiring Embedded Systems Engineer | Passionate about C Programming & Digital Security

📧 chennasainath2051@gmail.com
