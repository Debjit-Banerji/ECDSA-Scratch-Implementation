#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <cstring>
#include <iomanip>
// #include <winsock2.h>
// #include <ws2tcpip.h> // For inet_pton and other helper functions
#include <sys/types.h>
#include <stdexcept>
#include <unistd.h>

using namespace std;

using ull = unsigned long long;

// Class to handle very large numbers (256 bit numbers)
class BigNum
{
    // Variable to store the hexadecimal representation of the very large number
    string str = "";

    // Function to convert the hexadecimal string representation to decimal string representation
    void hexToDecimal()
    {
        // decimalValue = to_string(stoull(str, nullptr, 16));
        BIGNUM *bn = BN_new();
        if (!BN_hex2bn(&bn, str.c_str())) {
            BN_free(bn);
            throw invalid_argument("Invalid hexadecimal string.");
        }

        char *decimalStr = BN_bn2dec(bn);
        string temp_decimalValue(decimalStr);
        OPENSSL_free(decimalStr);
        BN_free(bn);

        this->decimalValue = temp_decimalValue;
    }

    // Function to convert the decimal string representation to hexadecimal string representation
    void decimalToHex()
    {
        // str = to_string(stoul(decimalValue, nullptr, 10));
        BIGNUM *bn = BN_new();
        if (!BN_dec2bn(&bn, decimalValue.c_str())) {
            BN_free(bn);
            throw invalid_argument("Invalid decimal string.");
        }

        char *hexStr = BN_bn2hex(bn);
        string temp_str(hexStr);
        OPENSSL_free(hexStr);
        BN_free(bn);

        this->str = temp_str;
    }

public:

    // Variable to store the decimal representation of the very large number
    string decimalValue = "";

    // Defining the default constructor for the class
    BigNum(){
        decimalValue = "0";
        decimalToHex();
    }

    // Defining the parameterized constructor for the class
    BigNum(string s, int f){
        if(f == 1){
            str = s;
            hexToDecimal();
        }
        else{
            decimalValue = s;
            decimalToHex();
        }
    }

    string get_str(){
        return str;
    }

    // Function to check equality between the current classes value and another BigNum value given to the function as an argument
    bool operator==(BigNum b)
    {
        return str == b.str;
    }

    // Function to add this class's BigNum value to another BigNum value given to the function as an argument
    BigNum operator+(BigNum b)
    {
        // // Storing the decimal string representations of the two BigNum values to temporary variables
        // string temp1 = decimalValue;
        // string temp2 = b.decimalValue;

        // // Initializing addition carry to 0
        // int c = 0;

        // // Calculating the size of each of the BigNum decimal string representations
        // int n1 = temp1.size();
        // int n2 = temp2.size();

        // reverse(temp1.begin(), temp1.end());
        // reverse(temp2.begin(), temp2.end());

        // // Defining an empty string to store the decimal string representation of the addition result
        // string res = "";
        // if(n1 >= n2){ // This is executed if the first value's length is greater than the second value's length
        //     ull i = 0;
        //     for(i = 0; i < n2; i++){
        //         int sum = ((temp1[i]-'0') + (temp2[i]-'0') + c);
        //         res += to_string(sum % 10);
        //         c = sum / 10;
        //     }

        //     while(i < n1){
        //         int sum = (temp1[i]-'0') + c;
        //         res += to_string(sum % 10);
        //         c = sum/10;
        //         i++;
        //     }

        //     while(c > 0){
        //         res += to_string(c);
        //         c = 0;
        //     }
        // }
        // else{ // This is executed if the second value's length is greater than the first value's length
        //     ull i = 0;
        //     for(i = 0; i < n1; i++){
        //         int sum = ((temp1[i]-'0') + (temp2[i]-'0') + c);
        //         res += to_string(sum % 10);
        //         c = sum / 10;
        //     }

        //     while(i < n2){
        //         int sum = (temp2[i]-'0') + c;
        //         res += to_string(sum % 10);
        //         c = sum/10;
        //         i++;
        //     }

        //     while(c > 0){
        //         res += to_string(c);
        //         c = 0;
        //     }
        // }

        // reverse(res.begin(), res.end());

        // // Initialising the BigNum variable to store the result of the addition operation
        // BigNum final_res(res, 0);

        // // Returning the calculated result for further steps
        // return final_res;

        // Defining a new variable to store the BIGNUM type of the decimal representation of the current object
        BIGNUM *temp1 = BN_new();
        if (!BN_dec2bn(&temp1, decimalValue.c_str())) {
            BN_free(temp1);
            throw runtime_error("Invalid decimal string (Add 1)");
        }

        // Defining a new variable to store the BIGNUM type of the decimal representation of the object given as a parameter
        BIGNUM *temp2 = BN_new();
        if (!BN_dec2bn(&temp2, b.decimalValue.c_str())) {
            BN_free(temp2);
            BN_free(temp1);
            throw runtime_error("Invalid decimal string (Add 2)");
        }

        // Defining a new variable to store the BIGNUM type of the decimal representation for the addition result and calculating the result
        BIGNUM* result = BN_new();
        if (!BN_add(result, temp1, temp2)) {
            BN_free(result);
            BN_free(temp1);
            BN_free(temp2);
            throw runtime_error("Addition failed");
        }

        // Converting the adiition result to the custom BigNum type
        char* res = BN_bn2dec(result);
        BigNum final_result(res, 0);
        OPENSSL_free(res);

        // Freeing necessary variables
        BN_free(result);
        BN_free(temp1);
        BN_free(temp2);

        // Returning the final result of the addition operation for further steps
        return final_result;
    }

    // Function to multiply this class's BigNum value with another BigNum value given to the function as an argument
    BigNum operator*(BigNum b)
    {
        // // Initialising variables to store the lengths of the BigNum values' decimal representations
        // ull n1 = decimalValue.size(), n2 = b.decimalValue.size();

        // // Initialising a variable to store the final result after multiplication
        // string res(n1 + n2, '0');
        
        // // Performing the multiplication
        // for (int i = n1-1; i >= 0; i--) {
        //     int carry = 0;
        //     for (int j = n2-1; j >= 0; j--) {
        //         int temp = (res[i + j + 1] - '0') + (decimalValue[i] - '0') * (b.decimalValue[j] - '0') + carry;
        //         res[i + j + 1] = (temp % 10) + '0';
        //         carry = temp / 10;
        //     }
        //     res[i] = carry + '0';
        // }
        
        // // Removing leading zeros in the resulatant string
        // size_t first_zero = res.find_first_not_of('0');

        // // If the resultant string is not equal to 0
        // if (first_zero != string::npos) {

        //     // Initialising a variable to store the final result as a BigNum variable and returning it for further steps
        //     BigNum final_res = BigNum(res.substr(first_zero), 0);
        //     return final_res;
        // }

        // // Initialising a variable to store the final result, i.e., 0 in this case, as a BigNum variable and returning it for further steps
        // BigNum final_res = BigNum();
        // return final_res;

        // Defining a new variable to store the BIGNUM object of the context variable which will be used during the calculation
        BN_CTX* context = BN_CTX_new();
        if (!context) {
            BN_CTX_free(context);
            throw runtime_error("Error creating Context Variable!");
        }

        // Defining a new variable to store the BIGNUM object of the decimal representation of the current object
        BIGNUM *temp1 = BN_new();
        if (!BN_dec2bn(&temp1, decimalValue.c_str())) {
            BN_free(temp1);
            BN_CTX_free(context);
            throw runtime_error("Invalid decimal string (Mul 1)");
        }

        // Defining a new variable to store the BIGNUM object of the decimal representation of object given as an argument
        BIGNUM* temp2 = BN_new();
        if(!BN_dec2bn(&temp2, b.decimalValue.c_str())){
            BN_free(temp2);
            BN_free(temp1);
            BN_CTX_free(context);
            throw runtime_error("Invalid decimal string (Mul 1)");
        }

        // Defining a new variable to store the result of the multiplication and performing the multiplication
        BIGNUM* result = BN_new();
        if(!BN_mul(result, temp1, temp2, context)){
            BN_CTX_free(context);
            BN_free(result);
            BN_free(temp1);
            BN_free(temp2);
            throw runtime_error("Error performing multiplication!");
        }

        // Converting the multiplication result to custom BigNum type
        char* res = BN_bn2dec(result);
        BigNum final_result(res, 0);
        OPENSSL_free(res);

        // Freeing necessary variables
        BN_CTX_free(context);
        BN_free(result);
        BN_free(temp1);
        BN_free(temp2);

        // Returning the final result of the multiplication operation for further steps
        return final_result;
    }

    // Function to calculate the modulus of this class's BigNum value with another BigNum value given to the function as an argument
    BigNum operator%(BigNum n) 
    {
        // // Initialising the BigNum variable to store the remainder
        // BigNum rem = BigNum();

        // // Iterating over each digit in the decimal string representation of the current class's value
        // for (char digit : decimalValue) {
        //     // Adding parts of the decimal string representation to the 'rem' value
        //     BigNum rem_temp = rem.operator*(BigNum("10", 0));
        //     rem = rem_temp.operator+(BigNum(string(1, digit), 0));
        //     if (rem.operator>=(n)) {
        //         // Calculating the modulus if 'rem's' value is more than or equal to 'n's' value
        //         BigNum rem_t1 = rem.operator/(n);
        //         BigNum rem_t2 = rem_t1.operator*(n);
        //         rem = rem.operator-(rem_t2);
        //     }
        // }

        // // Returning the calculated remainder (final modulus result) as a BigNum variable for further steps
        // return rem;

        // Defining a new variable to store the BIGNUM object of the context variable which will be used during the calculation
        BN_CTX* context = BN_CTX_new();
        if (!context) {
            BN_CTX_free(context);
            throw runtime_error("Error creating Context Variable!");
        }

        // Defining a new variable to store the BIGNUM object for the decimal representation of the current object
        BIGNUM* temp1 = BN_new();
        if (!BN_dec2bn(&temp1, decimalValue.c_str())) {
            BN_free(temp1);
            BN_CTX_free(context);
            throw runtime_error("Invalid decimal string (Mod 1)");
        }

        // Defining a new variable to store the BIGNUM object for the decimal representation of the object given as an argument
        BIGNUM* temp2 = BN_new();
        if(!BN_dec2bn(&temp2, n.decimalValue.c_str())){
            BN_free(temp2);
            BN_free(temp1);
            BN_CTX_free(context);
            throw runtime_error("Invalid decimal string (Mod 1)");
        }

        // Defining a new variable to store the remainder and performing the division to calculate the remainder
        BIGNUM* result = BN_new();
        if(!BN_div(NULL, result, temp1, temp2, context)){
            BN_CTX_free(context);
            BN_free(result);
            BN_free(temp1);
            BN_free(temp2);
            throw runtime_error("Modulus failed!");
        }

        // Running a while loop to ensure all mod values are positive
        while (BN_is_negative(result)) {
            BIGNUM* corrected = BN_new();
            if (!BN_add(corrected, result, temp2)) { // Add modulus to make it positive
                BN_CTX_free(context);
                BN_free(result);
                BN_free(temp1);
                BN_free(temp2);
                BN_free(corrected);
                throw runtime_error("Error correcting negative modulus result!");
            }
            result = corrected;
        }

        // Converting the modulus result to custom BigNum type
        char* res = BN_bn2dec(result);
        BigNum final_result(res, 0);
        OPENSSL_free(res);

        // Freeing necessary variables
        BN_CTX_free(context);
        BN_free(result);
        BN_free(temp1);
        BN_free(temp2);

        // Returning the final result of the modulus operation for further steps
        return final_result;
    }

    // Function to check if this class's BigNum value is greater than or equal to another BigNum value given to the function as an argument
    bool operator>=(BigNum b){
        string temp1 = decimalValue;
        string temp2 = b.decimalValue;
        if (temp1.size() > temp2.size()) return true;
        else if (temp1.size() < temp2.size()) return false;
        else return temp1 >= temp2;
    }
    
    // Function to check if this class's BigNum value is less than or equal to another BigNum value given to the function as an argument
    bool operator<=(BigNum b){
        string temp1 = decimalValue;
        string temp2 = b.decimalValue;
        if (temp1.size() > temp2.size()) return false;
        else if (temp1.size() < temp2.size()) return true;
        else return temp1 <= temp2;
    }

    // Function to check if this class's BigNum value is less than another BigNum value given to the function as an argument
    bool operator<(BigNum b){
        string temp1 = decimalValue;
        string temp2 = b.decimalValue;
        if (temp1.size() > temp2.size()) return false;
        else if (temp1.size() < temp2.size()) return true;
        else return temp1 < temp2;
    }

    // Function to get the inverse of this class's BigNum value using modulus as another BigNum value given to the function as an argument
    BigNum getInverse(BigNum n)  {

        // // Defining a new BigNum variable to point to the current class's BigNum value
        // BigNum a = *this;

        // // Initialising the coefficients for the iterative Extended Euclidean Algorithm
        // BigNum x0("1", 0), x1("0", 0);
        // BigNum y0("0", 0), y1("1", 0);

        // // Iterating while the value of n doesn't become 0
        // while (!(n == (BigNum("0", 0)))) {

        //     // Calculating the quotient and remainder of the division of a and n
        //     BigNum quotient = a.operator/(n);
        //     BigNum temp = a;
        //     a = n;
        //     n = temp.operator-(quotient.operator*(n));

        //     // Calculating the coefficients for the first terms of the formula
        //     temp = x0;
        //     x0 = x1;
        //     x1 = temp.operator-(quotient.operator*(x1));
            
        //     // Calculating the coefficients for the second terms of the formula
        //     temp = y0;
        //     y0 = y1;
        //     y1 = temp.operator-(quotient.operator*(y1));
        // }

        // if (x0.operator<(BigNum("0", 0))) { // Adding n to the previously found coefficient to make sure its in the range [0, n-1]
        //     x0 = x0.operator+(n);
        // }

        // // The coefficient indicates the inverse of a mod n, and so we are returning it for further steps
        // return x0;

        // Defining a new variable to store the BIGNUM object for the context variable which will be used for the calculation
        BN_CTX* context = BN_CTX_new();
        if (!context) {
            BN_CTX_free(context);
            throw runtime_error("Error creating BN_CTX");
        }

        // Defining a new variable to store the BIGNUM object for the decimal representation of the current object
        BIGNUM* temp1 = BN_new();
        if(!BN_dec2bn(&temp1, decimalValue.c_str())){
            BN_free(temp1);
            BN_CTX_free(context);
            throw runtime_error("Invalid decimal string (Inv 1)");
        }        

        // Defining a new variable to store the BIGNUM object for the decimal representation of the object given as an argument
        BIGNUM* temp2 = BN_new();
        if(!BN_dec2bn(&temp2, n.decimalValue.c_str())){
            BN_free(temp2);
            BN_free(temp1);
            BN_CTX_free(context);
            throw runtime_error("Invalid decimal string (Inv 1)");
        }

        // Defining a new variable to store the result of the modular inverse operation and performing the modular inverse operation
        BIGNUM* result = BN_new();
        if (!BN_mod_inverse(result, temp1, temp2, context)) {
            BN_free(result);
            BN_free(temp2);
            BN_free(temp1);
            BN_CTX_free(context);
            throw runtime_error("Error computing modular inverse");
        }

        // Converting the modular inverse result to BigNum type
        char* res = BN_bn2dec(result);
        BigNum final_result(res, 0);
        OPENSSL_free(res);

        // Freeing necessary variables
        BN_CTX_free(context);
        BN_free(temp1);
        BN_free(temp2);
        BN_free(result);

        // Returning the final result of the modular inverse function for further steps
        return final_result;
    }

    // Function to subtract another BigNum value given to the function as an argument, from this class's BigNum value
    BigNum operator-(BigNum b)  {

        // // Defining an empty string to store the final subtracted result in decimal representation
        // string res = "";

        // // Definining the variable for subtraction carry
        // ull carry = 0;

        // // Initialising variables with the lengths of the BigNum values' decimal representations
        // ull n1 = decimalValue.size();
        // ull n2 = b.decimalValue.size();

        // // Calculating the maximum length of the decimal representations
        // ull maxl = max(n1, n2);

        // // Adjusting the lengths of the two decimal representations according to the maximum length
        // string temp1 = string(maxl - n1, '0') + decimalValue;
        // string temp2 = string(maxl - n2, '0') + b.decimalValue;

        // // cout << temp1 << " " << temp2 << " " << maxl << endl;

        // // Iterating over all the decimal places for subtraction
        // for (int i = maxl - 1; i >= 0; i--) {
        //     // Calculating the difference between the two digits of the current decimal place and also subtracting carry (if there is any)
        //     int diff = (temp1[i] - '0') - (temp2[i] - '0') - carry;
        //     if (diff < 0) { // If the difference is negative, then we need to borrow from the next decimal place
        //         diff += 10;
        //         carry = 1;
        //     } else { // Else we update the carry to be 0
        //         carry = 0;
        //     }
        //     // Inserting the current decimal place difference into the result string
        //     res.insert(res.begin(), diff + '0');
        // }

        // if (res != "0"){
        //     size_t idx = res.find_first_not_of('0');
        //     res = res.substr(idx);
        // }        

        // // Initialising a BigNum variable to store the result of the subtraction and returning it for further steps
        // BigNum final_res(res, 0);
        // return final_res;

        // Defining a new variable to store the BIGNUM object for the decimal representation of the current object
        BIGNUM* temp1 = BN_new();
        if(!BN_dec2bn(&temp1, decimalValue.c_str())){
            BN_free(temp1);
            throw runtime_error("Invalid decimal string (Sub 1)");
        }

        // Defining a new variable to store the BIGNUM object for the decimal representation of the object given as an argument
        BIGNUM* temp2 = BN_new();
        if(!BN_dec2bn(&temp2, b.decimalValue.c_str())){
            BN_free(temp2);
            BN_free(temp1);
            throw runtime_error("Invalid decimal string (Sub 1)");
        }

        // Defining a new variable to store the result of the subtraction and performing the subtraction operation
        BIGNUM* result = BN_new();
        if(!BN_sub(result, temp1, temp2)){
            BN_free(result);
            BN_free(temp1);
            BN_free(temp2);
            throw runtime_error("Subtraction failed!");
        }

        // Converting the subtraction result to the custom BigNum type
        char* res = BN_bn2dec(result);
        BigNum final_result(res, 0);
        OPENSSL_free(res);

        // Freeing necessary variables
        BN_free(result);
        BN_free(temp1);
        BN_free(temp2);

        // Returning the final result of the subtraction function for further steps
        return final_result;
    }

    // Function to divide this class's BigNum value by another BigNum value given to the function as an argument
    BigNum operator/(BigNum b)  {

    //     // Initialising variables to store the dividend and the divisor for performing division in the next steps
    //     BigNum dividend = BigNum(decimalValue, 0);
    //     BigNum divisor = b;

    //     // Initialising the quotient to be 0
    //     ull quotient = 0;

    //     // Calculating the quotient
    //     while (dividend.operator>=(divisor)) {
    //         // Subtracting the divisor from the dividend
    //         dividend = dividend.operator-(divisor);
    //         // Updating the quotient value
    //         quotient += 1;
    //     }

    //     // Initialising a BigNum variable to store the final quotient and returning it for further steps
    //     BigNum final_quotient(to_string(quotient), 0);
    //     return final_quotient;

        // Initialising a new variable to store the BIGNUM object for the context variable which will be used later during the computation
        BN_CTX* context = BN_CTX_new();
        if (!context) {
            BN_CTX_free(context);
            throw runtime_error("Error initialising the context variable");
        }

        // Initialising variables to store the quotient and the remainder for performing division in the next steps
        BIGNUM* quotient = BN_new();
        BIGNUM* rem = BN_new();

        // Initialising a new variable to store the BIGNUM object for the decimal representation of the current object
        BIGNUM *temp1 = BN_new();
        if (!BN_dec2bn(&temp1, decimalValue.c_str())) {
            BN_free(temp1);
            BN_CTX_free(context);
            BN_free(quotient);
            BN_free(rem);
            throw runtime_error("Invalid decimal string (Div 1)");
        }

        // Initialising a new variable to store the BIGNUM object for the decimal representation of the object given as an argument
        BIGNUM* temp2 = BN_new();
        if (!BN_dec2bn(&temp2, b.decimalValue.c_str())) {
            BN_free(temp1);
            BN_free(temp2);
            BN_CTX_free(context);
            BN_free(quotient);
            BN_free(rem);
            throw runtime_error("Invalid decimal string (Div 1)");
        }

        // Performing the division operation
        if (!BN_div(quotient, rem, temp1, temp2, context)) {
            BN_CTX_free(context);
            BN_free(quotient);
            BN_free(rem);
            BN_free(temp1);
            BN_free(temp2);
            throw runtime_error("Division Failed!");
        }

        // Converting the quotient to the custom BigNum type
        char* quotientStr = BN_bn2dec(quotient);
        BigNum result(quotientStr, 0);
        OPENSSL_free(quotientStr);

        // Freeing necessary variables
        BN_CTX_free(context);
        BN_free(quotient);
        BN_free(rem);
        BN_free(temp1);
        BN_free(temp2);

        // Returning the final result of the division function for further steps
        return result;
    }

    // Function to calculate the square root of this class's BigNum value
    BigNum sqrt() const{

        // Initialising variables to store the low and high pointers for the binary search
        BigNum low("1", 0), high = *this;

        // Initialising variables to store the midpoint pointer and the final result of the binary search
        BigNum mid, res;

        // Performing the binary search to find the square root of the number
        while (low.operator<=(high)) {
            // Calculating the midpoint of the current range
            BigNum mid_temp(low.operator+(high));
            mid = mid_temp / (BigNum("2", 0));
            BigNum square = mid.operator*(mid);

            // If the mid point's square is equal to the current BigNum class's value
            if (square.operator==(*this)) {
                return mid;
            } else if (square.operator<(*this)) { // If the mid point is less than the current BigNum class's value
                low = mid.operator+(BigNum("1", 0));
                res = mid;
            } else { // If the mid point's square more than the current BigNum class's
                high = mid.operator-(BigNum("1", 0));
            }
        }

        // Returning the final result of the square root function for further steps
        return res;
    }

    string serialize() const{
        stringstream out;
        out << str << '|' << decimalValue;
        return out.str();
    }

    void deserialize(stringstream& in){
        getline(in, str, '|');
        getline(in, decimalValue, '|');
    }
};

class Point{
    BigNum x;
    BigNum y;
    bool even_y = false;
    bool is_identity = false;

public:
    // Default constructor
    Point(){
        x = BigNum();
        y = BigNum();
    }

    // Paremeterized constructor taking in x coordinate and calculting the y coordinate using the identity point of the ECC
    Point(BigNum &p, BigNum s, bool even_y) : x{s}, even_y{even_y} {
        generate_y(p);
    }

    // Parameterized constructor taking in x and y coordinates of the Point
    Point(BigNum x, BigNum y): x{x}, y{y} {}

    // Getter function for the X coordinate of the Point object
    BigNum get_x()
    {
        return x;
    }

    // Function to calculate the Y coordinate of the Point object using the identity point of the ECC
    BigNum generate_y(BigNum &p)
    {
        BigNum temp = (x * x) % p; // Calculates x^2
        BigNum temp2 = (temp * x) % p; // Calculates x^3
        BigNum y_temp = (temp2 + BigNum(to_string(7), 0)) % p; // Calculates x^3 + b
        BigNum y_temp_2 = (y_temp + (x * BigNum(to_string(0), 0)) % p) % p; // Calculates x^3 + a*x + b
        this->y = y_temp.sqrt() % p; // Calculates y from y^2 = x^3 + a*x + b
        return y;
    }

    // Getter function for the Y coordinate of the Point object
    BigNum get_y()
    {
        return y;
    }

    // Getter function for the parity of the Point object
    bool get_y_parity(){
        return even_y;
    }

    // Getter function for the identity flag of the Point object, which indicates whether the current point object is an identity or not
    bool getIdentity(){
        return is_identity;
    }

    // Setter function for the identity flag of the Point object, which is set according to whether the current point object is an identity or not
    void setIdentity(bool identity){
        is_identity = identity;
    }

    // Function to check the equality between two Point objects
    bool operator==(Point p)
    {
        if (x == p.x && y == p.y)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    // Function to serialize the Point object
    string serialize() const{
        stringstream out;
        out << x.serialize() << '|' << y.serialize();
        return out.str();
    }

    // Function to de-serialize the Point object
    void deserialize(stringstream& in) {
        x.deserialize(in);
        y.deserialize(in);
    }
    
};

class Curve
{
    // Initialising variable for the curve
    int a; // Parameter for the ECC equation
    int b; // Parameter for the ECC equation
    BigNum p; // Identity point of the ECC curve
    BigNum n; // Order of G
    Point G; // Generator Point of the Curve
    ull h; // Co-factor of the ECC curve

public:
    // Default constructor to initialize the curve with ECC Curve SECP256K1 values.
    Curve() {
        a = 0;
        b = 7;
        p = BigNum("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F", 1);
        n = BigNum("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141", 1);
        G = Point(BigNum("79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798", 1), BigNum("483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8", 1));
        h = (ull)1;
    }

    // Parameterized constructor to initialize the curve with custom values
    Curve(int a, int b, BigNum p, BigNum n, Point G, ull h): a(a), b(b), p(p), n(n), G(G), h(h) {}
    
    // Getter function for a
    int get_a()
    {
        return a;
    }

    // Getter function for b
    int get_b()
    {
        return b;
    }

    // Getter function for p
    BigNum get_p()
    {
        return p;
    }

    // Getter function for n
    BigNum get_n()
    {
        return n;
    }

    // Getter function for G
    Point get_G()
    {
        return G;
    }

    // Getter function for h
    ull get_h()
    {
        return h;
    }

    // Function to serialize the Curve object
    string serialize() const{
        stringstream out;
        out << a << '|' << b << '|' << p.serialize() << '|' << n.serialize() << '|' << G.serialize() << '|' << h;
        return out.str();
    }

    // Function to de-serialize the Curve object
    void deserialize(stringstream& in){
        in >> a;
        char c;
        in >> c;
        in >> b;
        in >> c;
        p.deserialize(in);
        n.deserialize(in);
        G.deserialize(in);
        in >> h;
        char temp;
        in >> temp;
    }
};

// Function to add two points on the ECC curve
Point pointAdd(Point p, Point q, Curve c)
{
    if(p.get_x().operator==(q.get_x()) && !(p.get_y() == q.get_y())){
        Point result;
        result.setIdentity(true);
        return result;
    
    } else if(p.get_x().operator==(q.get_x())){ // Case of Point Doubling
        // cout << "p1" << endl;
        BigNum x_squared = (p.get_x() * p.get_x()) % c.get_p();
        // cout << "p2" << " " << x_squared.decimalValue << endl;
        BigNum three_x_squared = (x_squared * BigNum("3", 0)) % c.get_p();
        // cout << "p3" << " " << three_x_squared.decimalValue << endl;
        BigNum den = ((p.get_y() * BigNum("2", 0)) % c.get_p()).getInverse(c.get_p());
        // cout << "p4" << " " << den.decimalValue << endl;
        BigNum slope = (three_x_squared * den) % c.get_p();
        // cout << "p5" << " " << slope.decimalValue << endl;
        BigNum new_x = (((slope * slope) % c.get_p()) - ((p.get_x() * BigNum("2", 0)) % c.get_p())) % c.get_p();
        // cout << "p6" << " " << new_x.decimalValue << endl;
        BigNum new_y = (((slope * ((p.get_x() - new_x) % c.get_p())) % c.get_p()) - p.get_y()) % c.get_p();
        // cout << "p7" << " " << new_y.decimalValue << endl;
        Point final_point(new_x,new_y);
        return final_point;

    } else { // Case of Adding two different points

        BigNum lambda_temp = (p.get_y() - q.get_y()) % c.get_p(); // Calculating y1 - y2
        BigNum den = ((p.get_x() - q.get_x()) % c.get_p()).getInverse(c.get_p());
        BigNum lambda = (lambda_temp * den) % c.get_p() ; // Calculating (y1 - y2)/(x1 - x2)
        BigNum x3_temp = (lambda * lambda) % c.get_p(); // Calculating lambda^2
        BigNum x3_temp_2 = (x3_temp - p.get_x()) % c.get_p(); // Calculating lambda^2 - x1
        BigNum x3 = (x3_temp_2 - q.get_x()) % c.get_p(); // Calculating lambda^2 - x1 - x2
        BigNum y3_temp = (lambda - ((p.get_x() - x3) % c.get_p())) % c.get_p(); // Calculating lambda*(x3 - x1)
        BigNum y3 = (y3_temp - p.get_y()) % c.get_p(); // Calculating lambda*(x1 - x3) - y1

        // Initialising a BigNum variable to store the result of the Point Addition and returning it for further steps
        Point final_point(x3,y3);
        return final_point;
    }
    
}

Point pointMultiply(Point p, BigNum m, Curve cur)
{
    Point res_p; // Defining a new variable to store the resultant point object after scalar multiplication of the given Point object
    Point temp_p = p; // Defining a new variable to store the temporary result after each scalar multiplication of the given Point object 

    // Defining a map to convert the hexdecimal values to binary values
    unordered_map<string, string> mp;
    mp["0"] = "0000";
    mp["1"] = "0001";
    mp["2"] = "0010";
    mp["3"] = "0011";
    mp["4"] = "0100";
    mp["5"] = "0101";
    mp["6"] = "0110";
    mp["7"] = "0111";
    mp["8"] = "1000";
    mp["9"] = "1001";
    mp["A"] = "1010";
    mp["B"] = "1011";
    mp["C"] = "1100";
    mp["D"] = "1101";
    mp["E"] = "1110";
    mp["F"] = "1111";

    // Storing the hexadecimal representation of the given scalar in a new variable
    string hexValue = m.get_str();

    // Defining a new variable to store the binary representation of the given scalar
    string binValue = "";

    // Creating the bbinary representation of the given scalar
    for(int i = 0; i < (hexValue).size(); i++){
        binValue += mp[string(1, hexValue[i])];
    }

    // Performing the Scalar Multiplication using the Exponential Multiplication and Point Add function
    for(int i = binValue.size()-1;i>-1;i--){
        if(binValue[i] == '1'){
            res_p = pointAdd(res_p, temp_p, cur);
        }
        temp_p = pointAdd(temp_p, temp_p, cur);
    }

    // Returning the resultant point after scalar multiplication
    return res_p;
}