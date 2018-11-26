#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal.h>
 
#define SS_PIN 10
#define RST_PIN 9
#define BOTAO1 2  //Botão PIN
#define SERVO 8   //Servo PIN


// Definição dos pinos que serão utilizados pelo displays
LiquidCrystal lcd(6, 7, 5, 4, 3, 2);

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Cria instancias MFRC522.
Servo servo8;                      // Define nome do servo
int pos = 90;                       // Posição inicial do servo
int portState = 0;                  // Definir status da porta 0 = Fechada, 1 = Aberta

void setup() 
{
  Serial.begin(9600);             // Inicializa a comunicação serial
  SPI.begin();                     // Inicializa o barramento SPI
  mfrc522.PCD_Init();              // Inicializa MFRC522
  servo8.attach(SERVO);            // Servo pin
  servo8.write(pos);               //Posição inicial do servo
  
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();

  //Define o número de colunas e linhas do LCD:  
  lcd.begin(16, 2);
  msgInicial();

}

void loop() 
{
  
  // Procurar por novos cartões
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Selecione um dos cartões
  if ( ! mfrc522.PICC_ReadCardSerial()){
    return;
  }
  
  //Mostra UID na serial
  Serial.print("UID tag :");
  String content= ""; //Variavel para guardar informações do cartão
  byte letter;

  // Pega informações do cartão e armazenar em content
  for (byte i = 0; i < mfrc522.uid.size; i++){
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();  //Tornar toda a informação em caixa alta
  
  if ((content.substring(1) == "86 80 27 25") || (content.substring(1) == "24 A1 B8 C3")) // Verifica autenticação dos cartões
  {
    if(portState == 0){ // Se estado da porta fechado abre e muda a variável para 1
      Serial.println("Acesso liberado!");
      Serial.println();
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("BEM VINDO!");
      lcd.setCursor(0,1);
      lcd.print("Acesso liberado!");
         
      for (pos = 90; pos <= 180; pos += 1) {
        servo8.write(pos);
        delay(10); // Espera 10 milessegundos para cada grau
      }

      delay(1000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Porta Destravada!");
      delay(2000);
      portState = 1;
      
      msgInicial();
            
    }else{ // Se estado da porta aberto, fecha e muda a variável para 1
      Serial.println("Travando a porta!");
      Serial.println();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Travando a porta");
      lcd.setCursor(0,1);
      lcd.print("Ate logo!");
               
      for (pos = pos; pos >= 90; pos -= 1) {
        servo8.write(pos);
        delay(10); // Espera 10 milessegundos para cada grau
      }

      delay(1000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Porta Travada!");
      portState = 0;
      delay(2000);
      msgInicial();

    }
  }
  
 else{ //Se o cartão for diferente dos cadastrados
    Serial.println(" Access denied");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Acesso Negado!");

    // Verifica se a porta está aberta, se sim, fecha automaticamente
    if(portState != 0){
      for (pos = pos; pos >= 90; pos -= 1) {
        servo8.write(pos);
        delay(10); // Espera 10 milessegundos para cada grau
      }
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Porta Travada!");
      portState = 0;
    }
    
    delay(2000);
    msgInicial();
  }
}

// Mensagem inicial para o display
void msgInicial(){
  lcd.clear();
  lcd.print(" Aproxime o seu");  
  lcd.setCursor(0,1);
  lcd.print("cartao do leitor");  
}
