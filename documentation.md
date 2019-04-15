# Arduino audio
## 1. Hardware platform
1. ATmega 328p  
Много разпространен микроконтролер с добра поддръжка от различни библиотеки. Също така има много налична информация за работата му, което го прави удобен за разработка. Проектът използва цялостната платка на Arduino (uno и nano), но всички разновидности с този микроконтролер са съвместими.  
Микроконтролерът използва модифицирана Харвард архитектура - avr. При нея липсват много екстри като модули за управление на паметта, защтото има малка нужда от тях.  
Важни негови характеристики, използвани в проекта:
    * 16 MHz честота - сравнително високата честота позволява достатъчно изчислителна мощ за възпроизвеждане на звук, макар и с ниско качество.  
    * Pulse Width Modulation (PWM) - 8 битова модулация с опция за правилна фаза, задвижван от осцилатора на микроконтролера. Макар че той няма цифрово-аналогов преобразувател, неговата функционалност може да се имитира с PWM и прост филтър.
    * UART комуникация - бърз и прост сериен интерфейс, идеален за лесно предаване на сравнително голям обем на иформация. В платките на Arduino един от тях може да се използва през USB порта.  

1. Сензори  
Комуникация с външния свят почти няма, заящото не е необходима. Въпреки това е включен бутон за пускане/спиране на възпроизваженето на звук.

1. Актуатори  
Има само един - говорителят. Той е свързан на PWM пина след low-pass филтър за преобразуване на сигнала в такъв, който е подходящ за възпроизвеждане на звук. По-голям говорител може да бъде свързан с усилвател.  

## 2. Software platform
1. avr-libc  
Проектът няма операционна система. Управляваме хардуера директно, защото е необходима голяма прецизност за възпроизвеждането на звук без изкривявания, тоест не използваме дори и библиотеката на Arduino. Библиотеката `avr-libc` ни предоставя дефиниции на регистри, вектори за прекъсвания и малък набор от помощни функции за улесняване на работата.

1. python  
За комуницаия с микроконтролера използваме python, защото е лесен за употреба и не се притесняваме за бързина. За улеснение на UART комуникацията използваме библиотеката `pyserial`.

## 3. SDD - Static view
   Static view се състои от 5 компонента: UART, Swap Buffer, Decoder, GPIO Driver и Low-pass Filter. Между UART и буфера се осъществява пренос на данни при интеръпт. Тази информация се предава на декодера. Той от своя страна изпраща информацията на GPIO драйвера, който я възпроизвежда на някой от пиновете, след което нискочестотният филтър премахва високочестотните компоненти от правоъгълният сигнал.
```
+--------+                  +---------------+                  +-----------+                   +---------------+
|  UART  | ---data_buff---> |  Swap Buffer  | ---buff_full---> |  Decoder  | ---gpio_write---> |  GPIO Driver  |
+--------+   [interrupt]    +---------------+    [event]       +-----------+   [registers]     +---------------+  
                                                                                                       |
                                                                                                       |
                                                                                                       |
                                                                                                       V
                                                                                             +-------------------+
                                                                                             |  Low-pass filter  |
                                                                                             +-------------------+
```

## 4. SDD - Dynamic View
   При изпращенето на нова информация по UART се създава интеръпт, който записва новите данни в Swap Buffer. След записването, буфера съобщава на декодера(Decoder) че нова информация е пристигнала. На декодера се дава адресът на записаната информация, докато буфере получава нов, за да може при пирстигането на нов inerrupt данните да не се презапишат върху още необработените такива. След като декодера пригоди сигнала в годен за изпращане по PWM, данните се възпроизвеждат.
```
   UART                Swap Buffer              Decoder              GPIO-PWM
    |                       |                      |                     |
 interrupt---data_buff[]--->|------buff_ready()--->|                     |
    |                       |<-----data_read()-----|                     |
    |                       |----data_buff[]---->decode--gpio_play()--->PWM
    |                       |                      |                     |
    |                       |                      |                     |
    ...
```
  