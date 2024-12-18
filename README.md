# Uzaktan Dosya Yönetim Sistemi

## Proje Hakkında
Bu proje, TCP/IP protokolü kullanılarak bir server-client mimarisi oluşturur.  
İstemci, dosya oluşturma, silme ve listeleme komutlarını sunucuya gönderir.  

## Kullanım Talimatları

### 1. Server'ı Başlatma
- `server.c` dosyasını derleyin ve çalıştırın:
  ```bash
  gcc server.c -o server -lpthread
  ./server
