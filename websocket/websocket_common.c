
#include "websocket_common.h"

//================================================== ���ܷ��� sha1��ϣ ==================================================

typedef struct SHA1Context{  
    unsigned Message_Digest[5];        
    unsigned Length_Low;               
    unsigned Length_High;              
    unsigned char Message_Block[64];   
    int Message_Block_Index;           
    int Computed;                      
    int Corrupted;                     
} SHA1Context;  

#define SHA1CircularShift(bits,word) ((((word) << (bits)) & 0xFFFFFFFF) | ((word) >> (32-(bits))))  

void SHA1ProcessMessageBlock(SHA1Context *context)
{  
    const unsigned K[] = {0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6 };  
    int         t;                  
    unsigned    temp;               
    unsigned    W[80];              
    unsigned    A, B, C, D, E;      
  
    for(t = 0; t < 16; t++) 
    {  
        W[t] = ((unsigned) context->Message_Block[t * 4]) << 24;  
        W[t] |= ((unsigned) context->Message_Block[t * 4 + 1]) << 16;  
        W[t] |= ((unsigned) context->Message_Block[t * 4 + 2]) << 8;  
        W[t] |= ((unsigned) context->Message_Block[t * 4 + 3]);  
    }  
      
    for(t = 16; t < 80; t++)  
        W[t] = SHA1CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);  
  
    A = context->Message_Digest[0];  
    B = context->Message_Digest[1];  
    C = context->Message_Digest[2];  
    D = context->Message_Digest[3];  
    E = context->Message_Digest[4];  
  
    for(t = 0; t < 20; t++) 
    {  
        temp =  SHA1CircularShift(5,A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];  
        temp &= 0xFFFFFFFF;  
        E = D;  
        D = C;  
        C = SHA1CircularShift(30,B);  
        B = A;  
        A = temp;  
    }  
    for(t = 20; t < 40; t++) 
    {  
        temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1];  
        temp &= 0xFFFFFFFF;  
        E = D;  
        D = C;  
        C = SHA1CircularShift(30,B);  
        B = A;  
        A = temp;  
    }  
    for(t = 40; t < 60; t++) 
    {  
        temp = SHA1CircularShift(5,A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];  
        temp &= 0xFFFFFFFF;  
        E = D;  
        D = C;  
        C = SHA1CircularShift(30,B);  
        B = A;  
        A = temp;  
    }  
    for(t = 60; t < 80; t++) 
    {  
        temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3];  
        temp &= 0xFFFFFFFF;  
        E = D;  
        D = C;  
        C = SHA1CircularShift(30,B);  
        B = A;  
        A = temp;  
    }  
    context->Message_Digest[0] = (context->Message_Digest[0] + A) & 0xFFFFFFFF;  
    context->Message_Digest[1] = (context->Message_Digest[1] + B) & 0xFFFFFFFF;  
    context->Message_Digest[2] = (context->Message_Digest[2] + C) & 0xFFFFFFFF;  
    context->Message_Digest[3] = (context->Message_Digest[3] + D) & 0xFFFFFFFF;  
    context->Message_Digest[4] = (context->Message_Digest[4] + E) & 0xFFFFFFFF;  
    context->Message_Block_Index = 0;  
} 

void SHA1Reset(SHA1Context *context)
{
    context->Length_Low             = 0;  
    context->Length_High            = 0;  
    context->Message_Block_Index    = 0;  
  
    context->Message_Digest[0]      = 0x67452301;  
    context->Message_Digest[1]      = 0xEFCDAB89;  
    context->Message_Digest[2]      = 0x98BADCFE;  
    context->Message_Digest[3]      = 0x10325476;  
    context->Message_Digest[4]      = 0xC3D2E1F0;  
  
    context->Computed   = 0;  
    context->Corrupted  = 0;  
}  
  
void SHA1PadMessage(SHA1Context *context)
{  
    if (context->Message_Block_Index > 55) 
    {  
        context->Message_Block[context->Message_Block_Index++] = 0x80;  
        while(context->Message_Block_Index < 64)  context->Message_Block[context->Message_Block_Index++] = 0;  
        SHA1ProcessMessageBlock(context);  
        while(context->Message_Block_Index < 56) context->Message_Block[context->Message_Block_Index++] = 0;  
    } 
    else 
    {  
        context->Message_Block[context->Message_Block_Index++] = 0x80;  
        while(context->Message_Block_Index < 56) context->Message_Block[context->Message_Block_Index++] = 0;  
    }  
    context->Message_Block[56] = (context->Length_High >> 24 ) & 0xFF;  
    context->Message_Block[57] = (context->Length_High >> 16 ) & 0xFF;  
    context->Message_Block[58] = (context->Length_High >> 8 ) & 0xFF;  
    context->Message_Block[59] = (context->Length_High) & 0xFF;  
    context->Message_Block[60] = (context->Length_Low >> 24 ) & 0xFF;  
    context->Message_Block[61] = (context->Length_Low >> 16 ) & 0xFF;  
    context->Message_Block[62] = (context->Length_Low >> 8 ) & 0xFF;  
    context->Message_Block[63] = (context->Length_Low) & 0xFF;  
  
    SHA1ProcessMessageBlock(context);  
} 

int SHA1Result(SHA1Context *context)
{
    if (context->Corrupted) 
    {  
        return 0;  
    }  
    if (!context->Computed) 
    {  
        SHA1PadMessage(context);  
        context->Computed = 1;  
    }  
    return 1;  
}  
  
  
void SHA1Input(SHA1Context *context,const char *message_array,unsigned length){  
    if (!length) 
        return;  
  
    if (context->Computed || context->Corrupted)
    {  
        context->Corrupted = 1;  
        return;  
    }  
  
    while(length-- && !context->Corrupted)
    {  
        context->Message_Block[context->Message_Block_Index++] = (*message_array & 0xFF);  
  
        context->Length_Low += 8;  
  
        context->Length_Low &= 0xFFFFFFFF;  
        if (context->Length_Low == 0)
        {  
            context->Length_High++;  
            context->Length_High &= 0xFFFFFFFF;  
            if (context->Length_High == 0) context->Corrupted = 1;  
        }  
  
        if (context->Message_Block_Index == 64)
        {  
            SHA1ProcessMessageBlock(context);  
        }  
        message_array++;  
    }  
}

/* 
int sha1_hash(const char *source, char *lrvar){// Main 
    SHA1Context sha; 
    char buf[128]; 
 
    SHA1Reset(&sha); 
    SHA1Input(&sha, source, strlen(source)); 
 
    if (!SHA1Result(&sha)){ 
        printf("SHA1 ERROR: Could not compute message digest"); 
        return -1; 
    } else { 
        memset(buf,0,sizeof(buf)); 
        sprintf(buf, "%08X%08X%08X%08X%08X", sha.Message_Digest[0],sha.Message_Digest[1], 
        sha.Message_Digest[2],sha.Message_Digest[3],sha.Message_Digest[4]); 
        //lr_save_string(buf, lrvar); 
         
        return strlen(buf); 
    } 
} 
*/  
char * sha1_hash(const char *source){   // Main  
    SHA1Context sha;  
    char *buf;//[128];  
  
    SHA1Reset(&sha);  
    SHA1Input(&sha, source, strlen(source));  
  
    if (!SHA1Result(&sha))
    {  
        printf("SHA1 ERROR: Could not compute message digest");  
        return NULL;  
    } 
    else 
    {  
        buf = (char *)malloc(128);  
        memset(buf, 0, 128);  
        sprintf(buf, "%08X%08X%08X%08X%08X", sha.Message_Digest[0],sha.Message_Digest[1],  
        sha.Message_Digest[2],sha.Message_Digest[3],sha.Message_Digest[4]);  
        //lr_save_string(buf, lrvar);  
          
        //return strlen(buf);  
        return buf;  
    }  
}  

int tolower(int c)   
{   
    if (c >= 'A' && c <= 'Z')   
    {   
        return c + 'a' - 'A';   
    }   
    else   
    {   
        return c;   
    }   
}

int htoi(const char s[], int start, int len)   
{   
    int i, j;   
    int n = 0;   
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X')) //�ж��Ƿ���ǰ��0x����0X 
    {   
        i = 2;   
    }   
    else   
    {   
        i = 0;   
    }   
    i+=start;  
    j=0;  
    for (; (s[i] >= '0' && s[i] <= '9')   
       || (s[i] >= 'a' && s[i] <= 'f') || (s[i] >='A' && s[i] <= 'F');++i)   
    {     
        if(j>=len)  
        {  
            break;  
        }  
        if (tolower(s[i]) > '9')   
        {   
            n = 16 * n + (10 + tolower(s[i]) - 'a');   
        }   
        else   
        {   
            n = 16 * n + (tolower(s[i]) - '0');   
        }   
        j++;  
    }   
    return n;   
}   

//================================================== ���ܷ���BASE64 ==================================================

//base64��/�����õĻ����ַ���
const char base64char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/******************************************************************************* 
* ����: base64_encode 
* ����: ascii����Ϊbase64��ʽ 
* �β�: bindata : ascii�ַ������� 
*            base64 : base64�ַ������ 
*          binlength : bindata�ĳ��� 
* ����: base64�ַ������� 
* ˵��: �� 
******************************************************************************/

int base64_encode( const unsigned char *bindata, char *base64, int binlength)
{
    int i, j;
    unsigned char current;
    for ( i = 0, j = 0 ; i < binlength ; i += 3 )
    {
        current = (bindata[i] >> 2) ;
        current &= (unsigned char)0x3F;
        base64[j++] = base64char[(int)current];
        current = ( (unsigned char)(bindata[i] << 4 ) ) & ( (unsigned char)0x30 ) ;
        if ( i + 1 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+1] >> 4) ) & ( (unsigned char) 0x0F );
        base64[j++] = base64char[(int)current];
        current = ( (unsigned char)(bindata[i+1] << 2) ) & ( (unsigned char)0x3C ) ;
        if ( i + 2 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+2] >> 6) ) & ( (unsigned char) 0x03 );
        base64[j++] = base64char[(int)current];
        current = ( (unsigned char)bindata[i+2] ) & ( (unsigned char)0x3F ) ;
        base64[j++] = base64char[(int)current];
    }
    base64[j] = '\0';
    return j;
}
/*******************************************************************************
 * 名称: base64_decode
 * 功能: base64格式解码为ascii
 * 形参: base64 : base64字符串输入
 *            bindata : ascii字符串输出
 * 返回: 解码出来的ascii字符串长度
 * 说明: 无
 ******************************************************************************/
int base64_decode( const char *base64, unsigned char *bindata)
{
    int i, j;
    unsigned char k;
    unsigned char temp[4];
    for ( i = 0, j = 0; base64[i] != '\0' ; i += 4 )
    {
        memset( temp, 0xFF, sizeof(temp) );
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i] )
                temp[0]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+1] )
                temp[1]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+2] )
                temp[2]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+3] )
                temp[3]= k;
        }
        bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2))&0xFC)) | \
                ((unsigned char)((unsigned char)(temp[1]>>4)&0x03));
        if ( base64[i+2] == '=' )
            break;
        bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4))&0xF0)) | \
                ((unsigned char)((unsigned char)(temp[2]>>2)&0x0F));
        if ( base64[i+3] == '=' )
            break;
        bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6))&0xF0)) | \
                ((unsigned char)(temp[3]&0x3F));
    }
    return j;
}

//==============================================================================================================
//================================================== websocket ==================================================
//==============================================================================================================

// ���ӷ�����
#define REPORT_LOGIN_CONNECT_TIMEOUT      1000                                                                        // ��¼���ӳ�ʱ���� 1000ms
#define REPORT_LOGIN_RESPOND_TIMEOUT      (1000 + REPORT_LOGIN_CONNECT_TIMEOUT)    // ��¼�ȴ���Ӧ��ʱ���� 1000ms
// ָ���
#define REPORT_ANALYSIS_ERR_RESEND_DELAY    500     // ���յ��ظ����ݵ�������ͨ��, ��ʱ һ��ʱ����ط�ָ��      ��λms
// ��������key�ĳ���
#define WEBSOCKET_SHAKE_KEY_LEN     16
/*
// websocket根据data[0]判别数据包类型
typedef enum{
    WCT_MINDATA = -20,     // 0x0����ʶһ���м����ݰ�
    WCT_TXTDATA = -19,      // 0x1����ʶһ��text�������ݰ�
    WCT_BINDATA = -18,     // 0x2����ʶһ��binary�������ݰ�
    WCT_DISCONN = -17,       // 0x8����ʶһ���Ͽ������������ݰ�
    WCT_PING = -16,     // 0x8����ʶһ���Ͽ������������ݰ�
    WCT_PONG = -15,    // 0xA����ʾһ��pong�������ݰ�
    WCT_ERR = -1,
    WCT_NULL = 0
}Websocket_CommunicationType;*/

/******************************************************************************* 
* ����: webSocket_getRandomString 
* ����: ��������ַ��� 
* �β�: *buf������ַ����洢�� 
*              len : ��������ַ������� 
* ����: �� 
* ˵��: ��
******************************************************************************/

void webSocket_getRandomString(unsigned char *buf, unsigned int len)
{
    unsigned int i;
    unsigned char temp;
    srand((int)time(0));
    for(i = 0; i < len; i++)
    {
        temp = (unsigned char)(rand()%256);
        if(temp == 0)   // �������Ҫ0, 0 ����Ŷ��ַ������ȵ��ж�
            temp = 128;
        buf[i] = temp;
    }
}
/******************************************************************************* 
* ����: webSocket_buildShakeKey 
* ����: client��ʹ����������������õ�key 
* �β�: *key��������ɵ�����key 
* ����: key�ĳ��� 
* ˵��: �� 
******************************************************************************/

int webSocket_buildShakeKey(unsigned char *key)
{
    unsigned char tempKey[WEBSOCKET_SHAKE_KEY_LEN] = {0};
    webSocket_getRandomString(tempKey, WEBSOCKET_SHAKE_KEY_LEN);
    return base64_encode((const unsigned char *)tempKey, (char *)key, WEBSOCKET_SHAKE_KEY_LEN);
}
/******************************************************************************* 
* ����: webSocket_buildRespondShakeKey 
* ����: server���ڽ���client�˵�key��,������Ӧ�õ�key 
* �β�: *acceptKey�����Կͻ��˵�key�ַ��� 
*         acceptKeyLen : ���� 
*          *respondKey :  �� acceptKey ֮����� GUID, ��sha1��ϣ, ��ת��base64�õ� respondKey 
* ����: respondKey�ĳ���(�϶���acceptKeyҪ��) 
* ˵��: �� 
******************************************************************************/

int webSocket_buildRespondShakeKey(unsigned char *acceptKey, unsigned int acceptKeyLen, unsigned char *respondKey)
{
    char *clientKey;  
    char *sha1DataTemp;  
    char *sha1Data;  
    int i, n;  
    const char GUID[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";  
    unsigned int GUIDLEN;
    
    if(acceptKey == NULL)  
        return 0;  
    GUIDLEN = sizeof(GUID);
    clientKey = (char *)calloc(1, sizeof(char)*(acceptKeyLen + GUIDLEN + 10));  
    memset(clientKey, 0, (acceptKeyLen + GUIDLEN + 10));
    //
    memcpy(clientKey, acceptKey, acceptKeyLen); 
    memcpy(&clientKey[acceptKeyLen], GUID, GUIDLEN);
    clientKey[acceptKeyLen + GUIDLEN] = '\0';
    //
    sha1DataTemp = sha1_hash(clientKey);  
    n = strlen(sha1DataTemp);  
    sha1Data = (char *)calloc(1, n / 2 + 1);  
    memset(sha1Data, 0, n / 2 + 1);  
   //
    for(i = 0; i < n; i += 2)  
        sha1Data[ i / 2 ] = htoi(sha1DataTemp, i, 2);      
    n = base64_encode((const unsigned char *)sha1Data, (char *)respondKey, (n / 2));
    //
    free(sha1DataTemp);
    free(sha1Data);
    free(clientKey);
    return n;
}
/******************************************************************************* 
* ����: webSocket_matchShakeKey 
* ����: client���յ����Է�������Ӧ��key�����ƥ��,����֤���ֳɹ� 
* �β�: *myKey��client����������ʱ������������key 
*            myKeyLen : ���� 
*          *acceptKey : ��������Ӧ��key 
*           acceptKeyLen : ���� 
* ����: 0 �ɹ�  -1 ʧ�� 
* ˵��: �� 
******************************************************************************/

int webSocket_matchShakeKey(unsigned char *myKey, unsigned int myKeyLen, unsigned char *acceptKey, unsigned int acceptKeyLen)
{
    int retLen;
    unsigned char tempKey[256] = {0};
    //
    retLen = webSocket_buildRespondShakeKey(myKey, myKeyLen, tempKey);
    //printf("webSocket_matchShakeKey :\r\n%d : %s\r\n%d : %s\r\n", acceptKeyLen, acceptKey, retLen, tempKey);
    //
    if(retLen != acceptKeyLen)
    {
        printf("webSocket_matchShakeKey : len err\r\n%s\r\n%s\r\n%s\r\n", myKey, tempKey, acceptKey);
        return -1;
    }
    else if(strcmp((const char *)tempKey, (const char *)acceptKey) != 0)
    {
        printf("webSocket_matchShakeKey : str err\r\n%s\r\n%s\r\n", tempKey, acceptKey);
        return -1;
    }
    return 0;
}
/******************************************************************************* 
* ����: webSocket_buildHttpHead 
* ����: ����client�����ӷ�����ʱ��httpЭ��ͷ, ע��websocket��GET��ʽ�� 
* �β�: *ip��Ҫ���ӵķ�����ip�ַ��� 
*          port : �������˿� 
*    *interfacePath : Ҫ���ӵĶ˿ڵ�ַ 
*      *shakeKey : ����key, �����������16λ�ַ��������base64��õ� 
*      *package : �洢������õ����� 
* ����: �� 
* ˵��: �� 
******************************************************************************/

void webSocket_buildHttpHead(char *ip, int port, char *interfacePath, unsigned char *shakeKey, char *package)
{
    const char httpDemo[] = "GET %s HTTP/1.1\r\n"
                                                "Connection: Upgrade\r\n"
                                                "Host: %s:%d\r\n"
                                                "Sec-WebSocket-Key: %s\r\n"
                                                "Sec-WebSocket-Version: 13\r\n"
                                                "Upgrade: websocket\r\n\r\n";
    sprintf(package, httpDemo, interfacePath, ip, port, shakeKey);
}
/******************************************************************************* 
* ����: webSocket_buildHttpRespond 
* ����: ����server�˻ظ�client���������httpЭ�� 
* �β�: *acceptKey������client������key 
*          acceptKeyLen : ���� 
*          *package : �洢 
* ����: �� 
* ˵��: �� 
******************************************************************************/

void webSocket_buildHttpRespond(unsigned char *acceptKey, unsigned int acceptKeyLen, char *package)
{
    const char httpDemo[] = "HTTP/1.1 101 Switching Protocols\r\n"
                                                "Upgrade: websocket\r\n"
                                                "Server: Microsoft-HTTPAPI/2.0\r\n"
                                                "Connection: Upgrade\r\n"
                                                "Sec-WebSocket-Accept: %s\r\n"
                                                "%s\r\n\r\n";  // ʱ��������        // ��ʽ�� "Date: Tue, 20 Jun 2017 08:50:41 CST\r\n"
    time_t now;
    struct tm *tm_now;
    char timeStr[256] = {0};
    unsigned char respondShakeKey[256] = {0};
    // ������Ӧ������key
    webSocket_buildRespondShakeKey(acceptKey, acceptKeyLen, respondShakeKey);   
    // ������Ӧʱ���ַ���
    time(&now);
    tm_now = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "Date: %a, %d %b %Y %T %Z", tm_now);
   // ��ɻظ���Ϣ
    sprintf(package, httpDemo, respondShakeKey, timeStr);
}
/******************************************************************************* 
* ����: webSocket_enPackage 
* ����: websocket�����շ��׶ε����ݴ��, ͨ��client��server�����ݶ�ҪisMask(����)����, ��֮server��clientȴ���� 
* �β�: *data��׼������������ 
*          dataLen : ���� 
*        *package : �����洢��ַ 
*        packageMaxLen : �洢��ַ���ó��� 
*          isMask : �Ƿ�ʹ������     1Ҫ   0 ��Ҫ 
*          type : ��������, �ɴ�����һ���ֽھ���, ����Ĭ�������ݴ���, ��0x81 
* ����: �����ĳ���(���ԭ���ݳ�2~16���ֽڲ���)      <=0 ���ʧ��  
* ˵��: �� 
******************************************************************************/

int webSocket_enPackage(unsigned char *data, unsigned int dataLen, unsigned char *package, unsigned int packageMaxLen, bool isMask, Websocket_CommunicationType type)
{
    unsigned char maskKey[4] = {0};     // ����
    unsigned char temp1, temp2;
    int count;
    unsigned int i, len = 0;
    
    if(packageMaxLen < 2)
        return -1;
    
    if(type == WCT_MINDATA)
        *package++ = 0x00;
    else if(type == WCT_TXTDATA)
        *package++ = 0x81;
    else if(type == WCT_BINDATA)
        *package++ = 0x82;
    else if(type == WCT_DISCONN)
        *package++ = 0x88;
    else if(type == WCT_PING)
        *package++ = 0x89;
    else if(type == WCT_PONG)
        *package++ = 0x8A;
    else
        return -1;
    //
    if(isMask)
        *package = 0x80;
    len += 1;
    //
    if(dataLen < 126)
    {
        *package++ |= (dataLen&0x7F);//ȡdataLen�ĺ�7���ֽ�
        len += 1;
    }
    else if(dataLen < 65536)
    {
        if(packageMaxLen < 4)
            return -1;
        *package++ |= 0x7E;
        *package++ = (char)((dataLen >> 8) & 0xFF);	//ȡ��λ
        *package++ = (unsigned char)((dataLen >> 0) & 0xFF);	//ȡ��λ
        len += 3;
    }
    else if(dataLen < 0xFFFFFFFF)
    {
        if(packageMaxLen < 10)
            return -1;
        *package++ |= 0x7F;
        *package++ = 0; //(char)((dataLen >> 56) & 0xFF);   // ���ݳ��ȱ����� unsigned int dataLen, ��ʱû����ô������
        *package++ = 0; //(char)((dataLen >> 48) & 0xFF);
        *package++ = 0; //(char)((dataLen >> 40) & 0xFF);
        *package++ = 0; //(char)((dataLen >> 32) & 0xFF);
        *package++ = (char)((dataLen >> 24) & 0xFF);        // ������͹���4GB������
        *package++ = (char)((dataLen >> 16) & 0xFF);
        *package++ = (char)((dataLen >> 8) & 0xFF);
        *package++ = (char)((dataLen >> 0) & 0xFF);
        len += 9;
    }
    //
    if(isMask)    // ����ʹ������ʱ, ʹ��������, maskKey[4]���κ������������, �߼�����
    {
        if(packageMaxLen < len + dataLen + 4)
            return -1;
        webSocket_getRandomString(maskKey, sizeof(maskKey));    // �����������
        *package++ = maskKey[0];
        *package++ = maskKey[1];
        *package++ = maskKey[2];
        *package++ = maskKey[3];
        len += 4;
        for(i = 0, count = 0; i < dataLen; i++)
        {
            temp1 = maskKey[count];
            temp2 = data[i];
            *package++ = (char)(((~temp1)&temp2) | (temp1&(~temp2)));   // ��������õ�����
            count += 1;
            if(count >= sizeof(maskKey))    // maskKey[4]ѭ��ʹ��
                count = 0;
        }
        len += i;
        *package = '\0';
    }
    else    // ����ûʹ������, ֱ�Ӹ������ݶ�
    {
        if(packageMaxLen < len + dataLen)
            return -1;
        memcpy(package, data, dataLen);
        package[dataLen] = '\0';
        len += dataLen;
    }
    //
    return len;
}
/******************************************************************************* 
* ����: webSocket_dePackage 
* ����: websocket�����շ��׶ε����ݽ��, ͨ��client��server�����ݶ�ҪisMask(����)����, ��֮server��clientȴ���� 
* �β�: *data����������� 
*          dataLen : ���� 
*        *package : �����洢��ַ 
*        packageMaxLen : �洢��ַ���ó��� 
*        *packageLen : ������ó��� 
* ����: ���ʶ����������� �� : txt����, bin����, ping, pong�� 
* ˵��: �� 
******************************************************************************/

int webSocket_dePackage(unsigned char *data, unsigned int dataLen, unsigned char *package, unsigned int packageMaxLen, unsigned int *packageLen)
{
    unsigned char maskKey[4] = {0};    // 掩码
    unsigned char temp1, temp2;
    char Mask = 0, type;
    int count, ret;
    unsigned int i, len = 0, dataStart = 2;
    if(dataLen < 2)
        return -1;
    
    type = data[0]&0x0F;
    
    if((data[0]&0x80) == 0x80)
    {
        if(type == 0x01) 
            ret = WCT_TXTDATA;
        else if(type == 0x02) 
            ret = WCT_BINDATA;
        else if(type == 0x08) 
            ret = WCT_DISCONN;
        else if(type == 0x09) 
            ret = WCT_PING;
        else if(type == 0x0A) 
            ret = WCT_PONG;
        else 
            return WCT_ERR;
    }
    else if(type == 0x00) 
        ret = WCT_MINDATA;
    else
        return WCT_ERR;
    //
    if((data[1] & 0x80) == 0x80)
    {
        Mask = 1;
        count = 4;
    }
    else
    {
        Mask = 0;
        count = 0;
    }
    //
    len = data[1] & 0x7F;
    //
    if(len == 126)
    {
        if(dataLen < 4)
            return WCT_ERR;
        len = data[2];
        len = (len << 8) + data[3];
        if(dataLen < len + 4 + count)
            return WCT_ERR;
        if(Mask)
        {
            maskKey[0] = data[4];
            maskKey[1] = data[5];
            maskKey[2] = data[6];
            maskKey[3] = data[7];
            dataStart = 8;
        }
        else
            dataStart = 4;
    }
    else if(len == 127)
    {
        if(dataLen < 10)
            return WCT_ERR;
        if(data[2] != 0 || data[3] != 0 || data[4] != 0 || data[5] != 0)    // ʹ��8���ֽڴ洢����ʱ, ǰ4λ����Ϊ0, װ������ô������...
            return WCT_ERR;
        len = data[6];
        len = (len << 8) + data[7];
        len = (len << 8) + data[8];
        len = (len << 8) + data[9];
        if(dataLen < len + 10 + count)
            return WCT_ERR;
        if(Mask)
        {
            maskKey[0] = data[10];
            maskKey[1] = data[11];
            maskKey[2] = data[12];
            maskKey[3] = data[13];
            dataStart = 14;
        }
        else
            dataStart = 10;
    }
    else
    {
        if(dataLen < len + 2 + count)
            return WCT_ERR;
        if(Mask)
        {
            maskKey[0] = data[2];
            maskKey[1] = data[3];
            maskKey[2] = data[4];
            maskKey[3] = data[5];
            dataStart = 6;
        }
        else
            dataStart = 2;
    }
    //
    if(dataLen < len + dataStart)
        return WCT_ERR;
    //
    if(packageMaxLen < len + 1)
        return WCT_ERR;
    //
    if(Mask)    // �������ʹ������ʱ, ʹ��������, maskKey[4]���κ������������, �߼�����
    {
        //printf("depackage : len/%d\r\n", len);
        for(i = 0, count = 0; i < len; i++)
        {
            temp1 = maskKey[count];
            temp2 = data[i + dataStart];
            *package++ =  (char)(((~temp1)&temp2) | (temp1&(~temp2)));  // ��������õ�����
            count += 1;
            if(count >= sizeof(maskKey))     // maskKey[4]ѭ��ʹ��
                count = 0;
            //printf("%.2X|%.2X|%.2X, ", temp1, temp2, *(package-1));
        }
        *package = '\0';
    }
    else    // �������ûʹ������, ֱ�Ӹ������ݶ�
    {
        memcpy(package, &data[dataStart], len);
        package[len] = '\0';
    }
    *packageLen = len;
    //
    return ret;
}
/******************************************************************************* 
* ����: webSocket_clientLinkToServer 
* ����: ��websocket����������http(Я������key), �Ժͷ�������������, ������ģʽ 
* �β�: *ip��������ip 
*          port : �������˿� 
*       *interface_path : �ӿڵ�ַ 
* ����: >0 �������Ӿ��      <= 0 ����ʧ�ܻ�ʱ, �����ѵ�ʱ�� ms 
* ˵��: �� 
******************************************************************************/

int webSocket_clientLinkToServer(char *ip, int port, char *interface_path)
{
    int ret, fd , timeOut;
    //int i;
	unsigned char loginBuf[512] = {0}, recBuf[512] = {0}, shakeKey[128] = {0}, *p;
	
	// zhd�������������ַ�ṹ��     
	struct sockaddr_in report_addr; 	
    memset(&report_addr,0,sizeof(report_addr)); 			// ���ݳ�ʼ��--����    
    report_addr.sin_family = AF_INET; 							// ����ΪIPͨ�� 
    report_addr.sin_addr.s_addr = inet_addr(ip);			// ������IP��ַ    
    report_addr.sin_port = htons(port); 							// �������˿ں�    
    
	//create unix socket  
    if((fd = socket(AF_INET,SOCK_STREAM, 0)) < 0) 
    {  
        printf("webSocket_login : cannot create socket\r\n");  
        return -1;  
    }
    /* 
      // ���� -----  ��������key �� ƥ�䷵��key
    webSocket_buildShakeKey(shakeKey); 
    printf("key1:%s\r\n", shakeKey);
    webSocket_buildRespondShakeKey(shakeKey, strlen(shakeKey), shakeKey);
    printf("key2:%s\r\n", shakeKey);
    */
    //������
    ret = fcntl(fd , F_GETFL , 0);
    fcntl(fd , F_SETFL , ret | O_NONBLOCK);
    
    //connect
    timeOut = 0;
	while(connect(fd , (struct sockaddr *)&report_addr,sizeof(struct sockaddr)) == -1)
	{
		if(++timeOut > REPORT_LOGIN_CONNECT_TIMEOUT)
		{
			printf("webSocket_login : %s:%d cannot connect !  %d\r\n" , ip, port, timeOut);  
			//
			sprintf(loginBuf, "webSocket_login : %s:%d cannot connect !  %d" , ip, port, timeOut);
			close(fd); 
		    return -timeOut;  
		}
		delayms(1);  //1ms 
	}
	
	//����httpЭ��ͷ
	memset(shakeKey, 0, sizeof(shakeKey));
	webSocket_buildShakeKey(shakeKey);  // ��������key
	
	memset(loginBuf, 0, sizeof(loginBuf));  // ����Э���
	webSocket_buildHttpHead(ip, port, interface_path, shakeKey, (char *)loginBuf);   
	// ����Э���
    ret = send(fd , loginBuf , strlen((const char*)loginBuf) , MSG_NOSIGNAL);     
    //
	//printf("\r\nconnect time : %d\r\nsend:\n%s\r\n" , timeOut, loginBuf); 
	
    while(1)
    {
		memset(recBuf , 0 , sizeof(recBuf));
		ret = recv(fd , recBuf , sizeof(recBuf) ,  MSG_NOSIGNAL);
		if(ret > 0)
		{
		    if(strncmp((const char *)recBuf, (const char *)"HTTP", strlen((const char *)"HTTP")) == 0)    // ���ص���http��Ӧ��Ϣ
            {
                //printf("\r\nlogin_recv : %d / %d\r\n%s\r\n" , ret, timeOut, recBuf); 
                if((p = (unsigned char *)strstr((const char *)recBuf, (const char *)"Sec-WebSocket-Accept: ")) != NULL)    // ��������ź�
                {
                    p += strlen((const char *)"Sec-WebSocket-Accept: ");
                    sscanf((const char *)p, "%s\r\n", p);
                    if(webSocket_matchShakeKey(shakeKey, strlen((const char *)shakeKey), p, strlen((const char *)p)) == 0)  // ���ֳɹ�, ���͵�¼���ݰ�
                        return fd; // ���ӳɹ�, �������Ӿ��fd
                    else    // �����źŲ���, �ط�Э���
                    {
                        ret = send(fd , loginBuf , strlen((const char*)loginBuf) , MSG_NOSIGNAL);     // �ط�Э���
                    }
                }
                else
                {
                    ret = send(fd , loginBuf , strlen((const char*)loginBuf) , MSG_NOSIGNAL);     // �ط�Э���
                }
            }/*
            else
            {
                // ��ʾ����
                if(recBuf[0] >= ' ' && recBuf[0] <= '~')
                { 
                    printf("\r\nlogin_recv : %d\r\n%s\r\n" , ret, recBuf); 
                }
                else
                {
                    printf("\r\nlogin_recv : %d\r\n" , ret); for(i = 0; i < ret; i++) printf("%.2X ", recBuf[i]); printf("\r\n");
                }
            }*/
		}
		else if(ret <= 0)
		{
		    ;
		}
		if(++timeOut > REPORT_LOGIN_RESPOND_TIMEOUT)
		{
		    close(fd); 
		    return -timeOut;
		}
		delayms(1);  //1ms
	}
    //
	close(fd); 
	return -timeOut;
}
/******************************************************************************* 
* ����: webSocket_serverLinkToClient 
* ����: �������ظ��ͻ��˵���������, �Խ���websocket���� 
* �β�: fd�����Ӿ�� 
*          *recvBuf : ���յ����Կͻ��˵�����(�ں�http��������) 
*              bufLen :  
* ����: =0 ����websocket���ӳɹ�        <0 ����websocket����ʧ�� 
* ˵��: �� 
******************************************************************************/

int webSocket_serverLinkToClient(int fd, char *recvBuf, unsigned int bufLen)
{
    char *p;
    int ret;
    char recvShakeKey[512], respondPackage[1024];
    
    if((p = strstr(recvBuf, "Sec-WebSocket-Key: ")) == NULL)
        return -1;
    p += strlen("Sec-WebSocket-Key: ");
    //
    memset(recvShakeKey, 0, sizeof(recvShakeKey));
    sscanf(p, "%s", recvShakeKey);      // 取得握手key
    ret = strlen(recvShakeKey);
    if(ret < 1)
        return -1;
    //
    memset(respondPackage, 0, sizeof(respondPackage));
    webSocket_buildHttpRespond(recvShakeKey, ret, respondPackage);
    //
    return send(fd, respondPackage, strlen(respondPackage), MSG_NOSIGNAL);
}
/******************************************************************************* 
* ����: webSocket_send 
* ����: websocket���ݻ�������ͷ��� 
* �β�: fd�����Ӿ�� 
*          *data : ���� 
*          dataLen : ���� 
*          mod : �����Ƿ�ʹ������, �ͻ��˵�����������ʹ������ģʽ 
*          type : ����ҪҪ��ʲôʶ��ͷ���ͷ���(txt, bin, ping, pong ...) 
* ����: ����send�ķ��� 
* ˵��: �� 
******************************************************************************/

int webSocket_send(int fd, unsigned char *data, unsigned int dataLen, bool mod, Websocket_CommunicationType type)
{
    unsigned char *webSocketPackage;
    unsigned int retLen, ret;
    //unsigned int i;
    
    //printf("webSocket_send : %d\r\n", dataLen);
    //---------- websocket���ݴ�� ----------
    webSocketPackage = (unsigned char *)calloc(1, sizeof(char)*(dataLen + 128));  
	memset(webSocketPackage, 0, (dataLen + 128));
    retLen = webSocket_enPackage(data, dataLen, webSocketPackage, (dataLen + 128), mod, type);
    /*
    printf("webSocket_send : %d\r\n" , retLen);
    for(i = 0; i < retLen; i ++)  printf("%.2X ", webSocketPackage[i]);
    printf("\r\n");*/
    ret = send(fd, webSocketPackage, retLen, MSG_NOSIGNAL);
    free(webSocketPackage);
    return ret;
}
/******************************************************************************* 
* ����: webSocket_recv 
* ����: websocket���ݽ��պͻ������ 
* �β�: fd�����Ӿ�� 
*          *data : ���ݽ��յ�ַ 
*          dataMaxLen : ������������󳤶� 
* ����: <= 0 û���յ���Ч����        > 0 �ɹ����ղ�������� 
* ˵��: �� 
******************************************************************************/

int webSocket_recv(int fd, unsigned char *data, unsigned int dataMaxLen)
{
    unsigned char *webSocketPackage, *recvBuf;
    int ret, ret2 = 0;
    unsigned int retLen = 0;
    //int i;
    
    recvBuf = (unsigned char *)calloc(1, sizeof(char)*dataMaxLen);  memset(recvBuf, 0, dataMaxLen);
    ret = recv(fd, recvBuf, dataMaxLen, 0/*MSG_NOSIGNAL*/);
    if(ret > 0)
    {
        //------------------------------------------------------------------------------------------------------------- �Ϳͻ��˵����Ӳ���
        if(strncmp(recvBuf, "GET", 3) == 0)
        {
            ret2 = webSocket_serverLinkToClient(fd, recvBuf, ret);
            free(recvBuf);
            if(ret2 < 0)
            {
                memset(data, 0, dataMaxLen);
                strcpy(data, "connect false !\r\n");
                return strlen("connect false !\r\n");
            }
            memset(data, 0, dataMaxLen);
            strcpy(data, "connect ...\r\n");
            return strlen("connect ...\r\n");
        }
        
        //------------------------------------------------------------------------------------------------------------- �������ݽ���
        
        //printf("webSocket_recv : %d\r\n", ret);
         //---------- websocket���ݴ�� ----------
        webSocketPackage = (unsigned char *)calloc(1, sizeof(char)*(ret + 128));  memset(webSocketPackage, 0, (ret + 128));
        ret2 = webSocket_dePackage(recvBuf, ret, webSocketPackage, (ret + 128), &retLen);
        if(ret2 == WCT_PING && retLen > 0)      // ����Ϊping��, �Զ���pong
        {
            webSocket_send(fd, webSocketPackage, retLen, true, WCT_PONG);
             // ��ʾ����
            printf("webSocket_recv : PING %d\r\n%s\r\n" , retLen, webSocketPackage); 
            free(recvBuf);
            free(webSocketPackage);
            return WCT_NULL;
        }
        else if(retLen > 0 && (ret2 == WCT_TXTDATA || ret2 == WCT_BINDATA || ret2 == WCT_MINDATA))  // ����Ϊ���ݰ�
        {
            memcpy(data, webSocketPackage, retLen);     // �ѽ����õ������ݸ��Ƴ�ȥ
            /*
           // ��ʾ����
            if(webSocketPackage[0] >= ' ' && webSocketPackage[0] <= '~')
            { 
                printf("\r\nwebSocket_recv : New Package StrFile ret2:%d/retLen:%d\r\n%s\r\n" , ret2, retLen, webSocketPackage); 
            }
            else
            {
                printf("\r\nwebSocket_recv : New Package BinFile ret2:%d/retLen:%d\r\n" , ret2, retLen); 
                for(i = 0; i < retLen; i++) printf("%.2X ", webSocketPackage[i]); printf("\r\n");
            }*/
            free(recvBuf);
            free(webSocketPackage);
            return retLen;
        }/*
        else
        {
             // ��ʾ����
            if(recvBuf[0] >= ' ' && recvBuf[0] <= '~') 
                printf("\r\nwebSocket_recv : ret:%d/ret2:%d/retLen:%d\r\n%s\r\n" , ret, ret2, retLen, recvBuf); 
            else 
            {
                printf("\r\nwebSocket_recv : ret:%d/ret2:%d/retLen:%d\r\n%s\r\n" , ret, ret2, retLen, recvBuf); 
                for(i = 0; i < ret; i++) printf("%.2X ", recvBuf[i]); printf("\r\n");
            }
        }*/
        free(recvBuf);
        free(webSocketPackage);
        return -ret;
    }
    else
    {
        free(recvBuf);
        return ret;
    }
}

void delayms(unsigned int ms)
{
    struct timeval tim;
    tim.tv_sec = ms/1000;
    tim.tv_usec = (ms%1000)*1000;
    select(0, NULL, NULL, NULL, &tim);
}


