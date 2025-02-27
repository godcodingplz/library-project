#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "admin123"
// 문자열 공백 제거 함수
void trimWhitespace(char* str);
// 회원 연결리스트 초기화 함수
void Client_linkedlist();
// 도서 연결리스트 초기화 함수
void Book_linkedlist();
// 대여 연결리스트 초기화 함수
void Borrow_linkedlist();
// 회원 정보를 파일에 저장하는 함수
void saveClientsToFile(const char* filename);
// 대여 목록 출력 함수
void my_borrow_list(int c_sid);
// 개인정보 수정 함수
void edit_my_info(int sid);
// 회원 탈퇴 함수
int del_my_info(int sid);
// 도서 정보를 파일에 저장하는 함수
void saveBooksToFile(const char* filename);
// 도서 등록 함수
void registerBook();
// 도서 삭제 함수
void deleteBook();
// 도서 대여 함수
void borrowBook();
// 대여 정보를 파일에 저장하는 함수
void saveBorrow(const char* filename);

// 도서 반납 함수

// 초기 UI 출력 함수
void displayInitialMenu();
// 회원 메뉴 UI 출력 함수
void displayMemberMenu();
// 관리자 메뉴 UI 출력 함수
void displayAdminMenu();
// 회원 등록 함수
void registerMember();
// 로그인 함수
int login(char* role, int* loggedInUser);
// 회원 목록 조회 함수
void viewMemberList();
// 문자열에서 부분 문자열 찾기 함수
int findSubstring(const char* str, const char* sub);
// 도서 검색 함수
void searchBook();
// 도서 반납 함수
void returnBook();
// 프로그램 시작 함수
void startProgram();



// 구조체 정의
typedef struct Client {
    int studentId;
    char password[100];
    char name[100];
    char address[100];
    char phonenum[100];
    struct Client* next;
} Client;

typedef struct Book {
    int bookId;
    char title[100];
    char publisher[100];
    char author[100];
    long long isbn;
    char location[50];
    char available; // Y or N
    struct Book* next;
} Book;

typedef struct Borrow {
    int studentId;
    int bookId;
    time_t borrowDate;
    time_t returnDate;
    struct Borrow* next;
} Borrow;

// 전역 변수
Client* a_head = NULL; // 회원 목록
Book* b_head = NULL;   // 도서 목록
Borrow* c_head = NULL; // 대여 목록

// 문자열 공백 제거 함수
void trimWhitespace(char* str) {
    char* end;
    while (*str == ' ' || *str == '\n' || *str == '\r') {
        str++;
    }
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }
}

void Client_linkedlist(){// 회원 연결리스트 초기화
    FILE* client_list = fopen("client.txt", "r");
    if (client_list == NULL) {
        printf("client.txt 파일 열기 실패\n");
        return;
    }

    while (!feof(client_list)) {
        Client* newClient = (Client*)malloc(sizeof(Client));
        if (fscanf(client_list, "%d | %99[^|] | %99[^|] | %99[^|] | %99[^\n]",
                   &newClient->studentId, newClient->password, newClient->name,
                   newClient->address, newClient->phonenum) == 5) {
            trimWhitespace(newClient->password);
            trimWhitespace(newClient->name);
            trimWhitespace(newClient->address);
            trimWhitespace(newClient->phonenum);
      
            Client* current = a_head;
            Client* prev = NULL;

            // 연결 리스트에 학번 순으로 삽입
            while (current && current->studentId < newClient->studentId) {
                prev = current;
                current = current->next;
            }

            if (prev == NULL) {
                newClient->next = a_head;
                a_head = newClient;
            } else {
                prev->next = newClient;
                newClient->next = current;
            }

        } else {
            free(newClient);
        }
    }
    fclose(client_list);
}

void Book_linkedlist(){// 도서 연결리스트 초기화
    FILE* book_list = fopen("book.txt", "r");
    if (book_list == NULL) {
        printf("book.txt 파일 열기 실패\n");
        return;
    }

    while (!feof(book_list)) {
        Book* newBook = (Book*)malloc(sizeof(Book));
        if (fscanf(book_list, "%d | %99[^|] | %99[^|] | %99[^|] | %lld | %49[^|] | %c\n",
                   &newBook->bookId, newBook->title, newBook->publisher, newBook->author,
                   &newBook->isbn, newBook->location, &newBook->available) == 7) {
            trimWhitespace(newBook->title);
            trimWhitespace(newBook->publisher);
            trimWhitespace(newBook->author);
            trimWhitespace(newBook->location);

            // 연결 리스트에 ISBN 순으로 삽입
            Book* current = b_head;
            Book* prev = NULL;

            while (current && current->isbn < newBook->isbn) {
                prev = current;
                current = current->next;
            }

            if (prev == NULL) {
                newBook->next = b_head;
                b_head = newBook;
            } else {
                prev->next = newBook;
                newBook->next = current;
            }

        } else {
            free(newBook);
        }
    }
    fclose(book_list);
}

void Borrow_linkedlist(){// 대여 연결리스트 초기화
    FILE* borrow_list = fopen("borrow.txt", "r");
    if (borrow_list == NULL) {
        printf("borrow.txt 파일 열기 실패\n");
        return;
    }

    while (!feof(borrow_list)) {
        Borrow* newBorrow = (Borrow*)malloc(sizeof(Borrow));
        if (fscanf(borrow_list, "%d | %d | %ld | %ld\n",
                   &newBorrow->studentId, &newBorrow->bookId,
                   &newBorrow->borrowDate, &newBorrow->returnDate) == 4) {
            newBorrow->next = c_head;
            c_head = newBorrow;
        } else {
            free(newBorrow);
        }
    }
    fclose(borrow_list);
}

void my_borrow_list(int c_sid){// 대여목록 출력
    // 도서번호, 도서명, 대여일자, 반납일자
    // borrow, book 파일 연결필요
    char* day[]={"일요일", "월요일", "화요일", "수요일", "목요일", "금요일", "토요일"};
    int brr_cnt = 0;
    Book* current_b;
    Borrow* current_c = c_head;

    while(current_c != NULL){
        if(current_c->studentId == c_sid){
            brr_cnt++; // 대여 기록이 있으면 brr_cnt 증가
            current_b = b_head;
            while(current_b->bookId != current_c->bookId){ current_b = current_b->next; }
            struct tm *t;
            printf("도서번호: %d\n", current_c->bookId);
            printf("도서명: %s\n", current_b->title);
            t=localtime(&current_c->borrowDate);
            printf("대여일자: %d년 %d월 %d일 %s\n", 1900+t->tm_year,1+t->tm_mon,t->tm_mday,day[t->tm_wday]);
            t=localtime(&current_c->returnDate);
            printf("반납일자:  %d년 %d월 %d일 %s\n", 1900+t->tm_year,1+t->tm_mon,t->tm_mday,day[t->tm_wday]);
        }
        current_c = current_c->next;
    }
    if(brr_cnt==0)
        printf("대여중인 도서가 없습니다\n");
}

void edit_my_info(int sid){// 회원정보 수정
        // 학번, 이름 제외 모든정보 수정

        char n_pw[20];
        char n_ad[100];
        char n_ph[20];
        getchar();
        printf(">>개인정보 수정<<\n");
        printf("비밀번호: ");
        scanf("%19[^\n]", n_pw);
        getchar();
        printf("주소: ");
        scanf("%99[^\n]", n_ad);
        getchar();
        printf("전화번호: ");
        scanf("%19[^\n]", n_ph);

        //a_head를 불러와서 수정된 내용을 적용
        Client* current = a_head;
        while(current != NULL){
                if(current->studentId == sid){
                        strcpy(current->password, n_pw);
                        strcpy(current->address, n_ad);
                        strcpy(current->phonenum, n_ph);
                        break;
                }
                current=current->next;
        }

        // 수정된 a_head를 파일에 저장
        FILE* file = fopen("client.txt", "w");
        if(file == NULL){
                printf("client.txt 파일 열기 실패\n");
                return;
        }
        current = a_head;
        while (current != NULL) {
                fprintf(file, "%d | %s | %s | %s | %s\n",
                                current->studentId,
                                current->password,
                                current->name,
                                current->address,
                                current->phonenum);
                current = current->next;
        }
        fclose(file);
}

int del_my_info(int sid){// 회원탈퇴
        // 대여한 도서가 없을때만 가능
        // c_head를 읽고 sid가 있는 노드를 삭제 후 client.txt에 덮어쓰기

        int brr_cnt = 0;
        Borrow* current_c = c_head;
        while(current_c != NULL){
                if(current_c->studentId == sid){
                        brr_cnt++;
                        break;
                }
                current_c = current_c->next;
        }

        char lc='o';
        if(brr_cnt>0){
                printf("대여중인 도서가 있습니다.\n대여중인 책을 먼저 반납해주세요.");
                return 0;
        }
        else{
                while(1){
                        printf("정말 진행하시겠습니까?(Y/N) ");
                        scanf(" %c",&lc);
                        if(lc=='Y' || lc=='y'){
                                break;
                        }
                        else if(lc == 'N' || lc == 'n'){
                                printf("회원 탈퇴가 취소되었습니다.\n");
                                return 0;
                        }
                }

                // 회원탈퇴 절차 시작
                FILE* file = fopen("client.txt", "w");

                if(file == NULL){
                        printf("client.txt 파일 열기 실패\n");
                        return 0;
                }

                // a_head에서 사용자를 찾아 노드제거
                Client* current = a_head;
                Client* prev = NULL;
                while(current != NULL){
                        if(current->studentId == sid){
                                if(prev == NULL){
                                        a_head = current->next;
                                }
                                else{
                                        prev->next = current->next;
                                }
                                break;
                        }
                        prev = current;
                        current = current->next;
                }

                current = a_head;
                // 변경적용된 a_head를 파일에 저장
                while(current != NULL){
                        fprintf(file, "%d | %s | %s | %s | %s\n",
                                        current->studentId,
                                        current->password,
                                        current->name,
                                        current->address,
                                        current->phonenum);
                        current = current->next;
                }
                fclose(file);

                printf("이용해주셔서 감사합니다.\n처음 화면으로 돌아가려면 엔터키를 누르십시오.");
                getchar();
                getchar();
                return 1;
        }
}

void saveClientsToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("client.txt 파일을 저장할 수 없습니다.\n");
        return;
    }

    Client* current = a_head;
    while (current) {
        fprintf(file, "%d | %s | %s | %s | %s\n",
                current->studentId, current->password, current->name,
                current->address, current->phonenum);
        current = current->next;
    }

    fclose(file);
    printf("회원 목록이 저장되었습니다.\n");
}

// 도서 저장 함수
void saveBooksToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("도서 파일을 저장할 수 없습니다.\n");
        return;
    }

    Book* current = b_head;
    while (current) {
        fprintf(file, "%d | %s | %s | %s | %lld | %s | %c\n",
                current->bookId, current->title, current->publisher,
                current->author, current->isbn, current->location,
                current->available);
        current = current->next;
    }

    fclose(file);
}

// 도서 등록
void registerBook() {
    Book* newBook = (Book*)malloc(sizeof(Book));
    if (!newBook) {
        printf("메모리 할당 실패\n");
        return;
    }

    // 사용자 입력
    printf(">> 도서 등록 <<\n");
    printf("도서명: ");
    scanf(" %[^\n]", newBook->title);
    printf("출판사: ");
    scanf(" %[^\n]", newBook->publisher);
    printf("저자명: ");
    scanf(" %[^\n]", newBook->author);
    printf("ISBN: ");
    scanf("%lld", &newBook->isbn);
    printf("소장처: ");
    scanf(" %[^\n]", newBook->location);

    // 자동 입력
    newBook->available = 'Y';

    // 도서번호는 연결리스트에서 마지막 도서번호 + 1
    int lastBookId = 0;
    Book* current = b_head;

    while (current) {
        if (current->bookId > lastBookId) {
            lastBookId = current->bookId;
        }
        current = current->next;
    }
    newBook->bookId = lastBookId + 1;

    printf("\n자동입력 사항\n");
    printf("대여가능 여부: %c\n", newBook->available);
    printf("도서번호: %d\n", newBook->bookId);
    printf("등록하시겠습니까? (Y/N): ");
    char confirm;
    scanf(" %c", &confirm);

    if (confirm == 'Y' || confirm == 'y') {
        // 정렬된 위치에 삽입
        Book* prev = NULL;
        current = b_head;
        while (current && newBook->isbn > current->isbn) {
            prev = current;
            current = current->next;
        }

        if (prev == NULL) {
            // 맨 앞에 삽입
            newBook->next = b_head;
            b_head = newBook;
        } else {
            // 중간 또는 맨 뒤에 삽입
            prev->next = newBook;
            newBook->next = current;
        }

        // 파일 저장
        saveBooksToFile("book.txt");

        printf("도서가 등록되었습니다.\n");
    } else {
        free(newBook);
        printf("도서 등록이 취소되었습니다.\n");
    }
}

// 도서 삭제
void deleteBook() {
    int searchBy;
    char searchQuery[100];
    long long searchIsbn;

    printf(">> 도서 삭제 <<\n");
    printf("1. 도서명 검색\n");
    printf("2. ISBN 검색\n");
    printf("검색 번호를 입력하세요: ");
    scanf("%d", &searchBy);

    Book* current = b_head;
    Book* matchedBooks[100]; // 검색 결과 저장
    int matchedCount = 0;

    // 검색 옵션에 따라 도서를 검색
    if (searchBy == 1) {
        printf("도서명을 입력하세요: ");
        scanf(" %[^\n]", searchQuery);
        while (current) {
            if (strcmp(current->title, searchQuery) == 0) {
                matchedBooks[matchedCount++] = current;
            }
            current = current->next;
        }
    } else if (searchBy == 2) {
        printf("ISBN을 입력하세요: ");
        scanf("%lld", &searchIsbn);
        while (current) {
            if (current->isbn == searchIsbn) {
                matchedBooks[matchedCount++] = current;
            }
            current = current->next;
        }
    } else {
        printf("잘못된 입력입니다.\n");
        return;
    }

    if (matchedCount == 0) {
        printf("검색 결과가 없습니다.\n");
        return;
    }

    // 검색 결과 출력
    printf("\n>> 검색 결과 <<\n");
    for (int i = 0; i < matchedCount; i++) {
        int isBorrowed = 0;
        Borrow* borrowCurrent = c_head;

        // 대여 중인지 확인
        while (borrowCurrent) {
            if (borrowCurrent->bookId == matchedBooks[i]->bookId) {
                isBorrowed = 1;
                break;
            }
            borrowCurrent = borrowCurrent->next;
        }

        printf("도서번호: %d (삭제 가능 여부: %c)\n", matchedBooks[i]->bookId, isBorrowed ? 'N' : 'Y');
        printf("도서명: %s\n", matchedBooks[i]->title);
        printf("출판사: %s\n", matchedBooks[i]->publisher);
        printf("저자명: %s\n", matchedBooks[i]->author);
        printf("ISBN: %lld\n", matchedBooks[i]->isbn);
        printf("소장처: %s\n\n", matchedBooks[i]->location);
    }

    printf("삭제할 도서의 번호를 입력하세요: ");
    int deleteId;
    scanf("%d", &deleteId);

    // 삭제 작업
    current = b_head;
    Book* prev = NULL;

    while (current) {
        if (current->bookId == deleteId) {
            // 삭제 가능 여부 확인
            Borrow* borrowCurrent = c_head;
            while (borrowCurrent) {
                if (borrowCurrent->bookId == current->bookId) {
                    printf("이 도서는 대여 중이므로 삭제할 수 없습니다.\n");
                    return;
                }
                borrowCurrent = borrowCurrent->next;
            }

            // 연결 리스트에서 도서 삭제
            if (prev) {
                prev->next = current->next;
            } else {
                b_head = current->next;
            }
            free(current);

            // 파일 업데이트
            saveBooksToFile("book.txt");

            printf("도서가 삭제되었습니다.\n");
            return;
        }
        prev = current;
        current = current->next;
    }

    printf("삭제할 도서를 찾을 수 없습니다.\n");
}

// 도서 대여
void borrowBook() {
    int searchBy;
    char searchQuery[100];
    long long searchIsbn;

    printf(">> 도서 대여 <<\n");
    printf("1. 도서명 검색\n");
    printf("2. ISBN 검색\n");
    printf("검색 번호를 입력하세요: ");
    scanf("%d", &searchBy);

    Book* current = b_head;
    Book* matchedBooks[100]; // 검색 결과 저장
    int matchedCount = 0;

    // 검색 옵션에 따라 도서 검색
    if (searchBy == 1) {
        printf("도서명을 입력하세요: ");
        scanf(" %[^\n]", searchQuery);
        while (current) {
            if (strcmp(current->title, searchQuery) == 0) {
                matchedBooks[matchedCount++] = current;
            }
            current = current->next;
        }
    } else if (searchBy == 2) {
        printf("ISBN을 입력하세요: ");
        scanf("%lld", &searchIsbn);
        while (current) {
            if (current->isbn == searchIsbn) {
                matchedBooks[matchedCount++] = current;
            }
            current = current->next;
        }
    } else {
        printf("잘못된 입력입니다.\n");
        return;
    }

    if (matchedCount == 0) {
        printf("검색 결과가 없습니다.\n");
        return;
    }

    // 검색 결과 출력
    printf("\n>> 검색 결과 <<\n");
    for (int i = 0; i < matchedCount; i++) {
        printf("도서번호: %d (대여 가능 여부: %c)\n", matchedBooks[i]->bookId, matchedBooks[i]->available);
        printf("도서명: %s\n", matchedBooks[i]->title);
        printf("출판사: %s\n", matchedBooks[i]->publisher);
        printf("저자명: %s\n", matchedBooks[i]->author);
        printf("ISBN: %lld\n", matchedBooks[i]->isbn);
        printf("소장처: %s\n\n", matchedBooks[i]->location);
    }

    int bookId, studentId;
    printf("학번을 입력하세요: ");
    scanf("%d", &studentId);

    printf("도서번호를 입력하세요: ");
    scanf("%d", &bookId);

    // 학번 유효성 확인
    Client* client = a_head;
    while (client) {
        if (client->studentId == studentId) break;
        client = client->next;
    }

    if (!client) {
        printf("유효하지 않은 학번입니다.\n");
        return;
    }

    // 대여 처리
    for (int i = 0; i < matchedCount; i++) {
        if (matchedBooks[i]->bookId == bookId) {
            if (matchedBooks[i]->available == 'N') {
                printf("이 도서는 대여 중입니다.\n");
                return;
            }

            printf("이 도서를 대여합니까? (Y/N): ");
            char confirm;
            scanf(" %c", &confirm);

            if (confirm == 'Y' || confirm == 'y') {
                // 대여 상태 변경
                matchedBooks[i]->available = 'N';

                // 대여 정보 추가
                Borrow* newBorrow = (Borrow*)malloc(sizeof(Borrow));
                newBorrow->studentId = studentId;
                newBorrow->bookId = bookId;
                newBorrow->borrowDate = time(NULL);
                newBorrow->returnDate = newBorrow->borrowDate + 30 * 24 * 60 * 60;
                newBorrow->next = c_head;
                c_head = newBorrow;

                // 파일 저장
                saveBooksToFile("book.txt");
                FILE* borrowFile = fopen("borrow.txt", "a");
                if (borrowFile) {
                    fprintf(borrowFile, "%d | %d | %ld | %ld\n",
                            studentId, bookId, newBorrow->borrowDate, newBorrow->returnDate);
                    fclose(borrowFile);
                }

                printf("도서가 대여되었습니다.\n");
                return;
            } else {
                printf("대여가 취소되었습니다.\n");
                return;
            }
        }
    }

    printf("대여할 도서를 찾을 수 없습니다.\n");
}

// 대여 목록을 borrow.txt 파일에 저장하는 함수
void saveBorrow(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("borrow.txt 파일을 저장할 수 없습니다.\n");
        return;
    }

    Borrow* current = c_head;
    while (current) {
        fprintf(file, "%d | %d | %ld | %ld\n",
                current->studentId, current->bookId,
                current->borrowDate, current->returnDate);
        current = current->next;
    }

    fclose(file);
}


// 도서 반납 함수


// UI 출력 함수들
void displayInitialMenu() {
    printf("\n>> 도서관 서비스 <<\n");
    printf("1. 회원 가입\n");
    printf("2. 로그인\n");
    printf("3. 프로그램 종료\n");
    printf("선택: ");
}

void displayMemberMenu() {
    printf("\n>> 회원 메뉴 <<\n");
    printf("1. 도서 검색\n");
    printf("2. 내 대여 목록\n");
    printf("3. 개인정보 수정\n");
    printf("4. 회원 탈퇴\n");
    printf("5. 로그아웃\n");
    printf("6. 프로그램 종료\n");
    printf("선택: ");
}

void displayAdminMenu() {
    printf("\n>> 관리자 메뉴 <<\n");
    printf("1. 도서 등록\n");
    printf("2. 도서 삭제\n");
    printf("3. 도서 대여\n");
    printf("4. 도서 반납\n");
    printf("5. 도서 검색\n");
    printf("6. 회원 목록\n");
    printf("7. 로그아웃\n");
    printf("8. 프로그램 종료\n");
    printf("선택: ");
}

void registerMember() {
    Client* newClient = (Client*)malloc(sizeof(Client));
    if (!newClient) {
        printf("메모리 할당 실패\n");
        return;
    }

    printf(">> 회원 가입 <<\n");
    printf("학번 (8자리): ");
    scanf("%d", &newClient->studentId);

    Client* current = a_head;
    while (current) {
        if (current->studentId == newClient->studentId) {
            printf("이미 존재하는 학번입니다. 회원가입을 취소합니다.\n");
            free(newClient);
            return;
        }
        current = current->next;
    }

    printf("비밀번호: ");
    scanf(" %[^\n]", newClient->password);
    printf("이름: ");
    scanf(" %[^\n]", newClient->name);
    printf("주소: ");
    scanf(" %[^\n]", newClient->address);
    printf("전화번호: ");
    scanf(" %[^\n]", newClient->phonenum);

    trimWhitespace(newClient->password);
    trimWhitespace(newClient->name);
    trimWhitespace(newClient->address);
    trimWhitespace(newClient->phonenum);

    Client* prev = NULL;
    current = a_head;

    while (current && current->studentId < newClient->studentId) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        newClient->next = a_head;
        a_head = newClient;
    } else {
        prev->next = newClient;
        newClient->next = current;
    }

    saveClientsToFile("client.txt");
    printf("회원가입이 완료되었습니다.\n");
}

int login(char* role, int* loggedInUser) {
    char username[50], password[20];

    printf("아이디 (학번): ");
    scanf("%49s", username);

    printf("비밀번호: ");
    scanf("%19s", password);

    trimWhitespace(username);
    trimWhitespace(password);

    if (strcmp(username, ADMIN_USERNAME) == 0 && strcmp(password, ADMIN_PASSWORD) == 0) {
        strcpy(role, "admin");
        printf("관리자 로그인 성공\n");
        return 1;
    }

    Client* client = a_head;
    while (client) {
        char trimmedPassword[100];
        sprintf(trimmedPassword, "%s", client->password);
        trimWhitespace(trimmedPassword);

        if (atoi(username) == client->studentId && strcmp(password, trimmedPassword) == 0) {
            strcpy(role, "member");
            *loggedInUser=client->studentId;
            printf("회원 로그인 성공\n");
            return 1;
        }
        client = client->next;
    }

    printf("로그인 실패\n");
    return 0;
}

// (관리자 전용) 회원 목록 조회 함수 
void viewMemberList() {
    int listChoice;
    char searchTerm[100];

    while (1) {
        printf("\n>> 회원 목록 <<\n");
        printf("1. 이름 검색 ");
        printf("2. 학번 검색\n");
        printf("3. 전체 검색 ");
        printf("4. 이전 메뉴\n");
        printf("선택: ");
        scanf("%d", &listChoice);

        if (listChoice == 4) {
            // 이전 메뉴로 돌아가기
            return;
        }

        Client* current = a_head;
        int found = 0;

        if (listChoice == 1) {
            // 이름 검색
            printf("검색할 이름을 입력하세요: ");
            scanf(" %[^\n]", searchTerm);

            while (current) {
                // 부분 문자열 검색을 할지 정확 일치 검색을 할지 명세에 따라 결정
                // 여기서는 정확 일치로 가정합니다. 부분 문자열 검색이 필요하면 strstr 사용
                if (strcmp(current->name, searchTerm) == 0) {
                    printf("학번: %d | 이름: %s | 주소: %s | 전화번호: %s\n",
                           current->studentId, current->name, current->address, current->phonenum);
                    found = 1;
                }
                current = current->next;
            }

        } else if (listChoice == 2) {
            // 학번 검색
            int searchId;
            printf("검색할 학번을 입력하세요: ");
            scanf("%d", &searchId);

            while (current) {
                if (current->studentId == searchId) {
                    printf("학번: %d | 이름: %s | 주소: %s | 전화번호: %s\n",
                           current->studentId, current->name, current->address, current->phonenum);
                    found = 1;
                    // 학번은 유일하다고 가정하면 break할 수도 있지만
                    // 일단은 break 없이 여러개 있을 수도 있다고 가정
                }
                current = current->next;
            }

        } else if (listChoice == 3) {
            // 전체 검색 (모든 회원 출력)
            if (!current) {
                printf("등록된 회원이 없습니다.\n");
            } else {
                while (current) {
                    printf("학번: %d | 이름: %s | 주소: %s | 전화번호: %s\n",
                           current->studentId, current->name, current->address, current->phonenum);
                    current = current->next;
                }
            }
            found = 1; // 전체 출력 시 항상 뭔가를 시도함
        } else {
            printf("잘못된 입력입니다.\n");
            continue;
        }

        if (!found && (listChoice == 1 || listChoice == 2)) {
            printf("검색 결과가 없습니다.\n");
        }
    }
}

// 부분 문자열 검색 함수 
int findSubstring(const char* str, const char* sub) {
    if (*sub == '\0') return 1; // 검색어가 빈 문자열이면 항상 매칭
    for (const char* p = str; *p; p++) {
        const char* q = p;
        const char* r = sub;
        while (*q && *r && (*q == *r)) {
            q++;
            r++;
        }
        if (*r == '\0') {
            // sub의 끝까지 매칭되었다면 성공
            return 1;
        }
    }
    return 0;
}

void searchBook() {
    while (1) {
        int searchBy;
        char searchQuery[100];
        long long searchIsbn;
        int foundCount = 0;

        printf("\n>> 도서 검색 <<\n");
        printf("1. 도서명 검색 ");
        printf("2. 출판사 검색\n");
        printf("3. ISBN 검색 ");
        printf("4. 저자명 검색\n");
        printf("5. 전체 검색 ");
        printf("6. 이전 메뉴\n");
        printf("검색 번호를 입력하세요: ");
        scanf("%d", &searchBy);

        if (searchBy == 6) {
            // 이전 메뉴로 돌아가기
            return; // 함수 종료 -> 관리자 메뉴로 복귀
        }

        // ISBN 검색(3번) 또는 전체 검색(5번)이 아닌 경우 검색어 입력
        if (searchBy != 3 && searchBy != 5 && searchBy >= 1 && searchBy <= 4) {
            printf("검색어를 입력하세요: ");
            scanf(" %[^\n]", searchQuery);
        }

        // ISBN 검색(3번)인 경우 ISBN 입력
        if (searchBy == 3) {
            printf("ISBN을 입력하세요: ");
            scanf("%lld", &searchIsbn);
        }

        Book* current = b_head;
        foundCount = 0;

        while (current) {
            int match = 0; // 현재 도서가 검색 조건을 만족하는지 여부
            switch (searchBy) {
                case 1: // 도서명 검색
                    if (findSubstring(current->title, searchQuery)) match = 1;
                    break;
                case 2: // 출판사 검색
                    if (findSubstring(current->publisher, searchQuery)) match = 1;
                    break;
                case 3: // ISBN 검색(정확 일치)
                    if (current->isbn == searchIsbn) match = 1;
                    break;
                case 4: // 저자명 검색
                    if (findSubstring(current->author, searchQuery)) match = 1;
                    break;
                case 5: // 전체 검색 (조건 없이 모두 출력)
                    match = 1;
                    break;
                default:
                    printf("잘못된 입력입니다.\n");
                    break;
            }

            if (match) {
                // 해당 ISBN을 가진 모든 도서를 조사하여 총 권수/대여 중인 권수를 계산
                int totalCopies = 0;
                int borrowedCopies = 0;
                Book* temp = b_head;
                while (temp) {
                    if (temp->isbn == current->isbn) {
                        totalCopies++;
                        if (temp->available == 'N') {
                            borrowedCopies++;
                        }
                    }
                    temp = temp->next;
                }

                // 도서 정보 출력
                printf("\n도서번호: %d\n", current->bookId);
                printf("도서명: %s\n", current->title);
                printf("출판사: %s\n", current->publisher);
                printf("저자명: %s\n", current->author);
                printf("ISBN: %lld\n", current->isbn);
                printf("소장처: %s\n", current->location);

                // 대여가능 여부와 (대여/총권수) 표시
                printf("대여가능 여부: %c(%d/%d)\n", current->available, borrowedCopies, totalCopies);

                foundCount++;
            }
            current = current->next;
        }

        if (foundCount == 0) {
            printf("검색 결과가 없습니다.\n");
        }

        // 검색이 끝나도 바로 return하지 않고 위로 돌아가서 다시 검색 메뉴를 출력
        // 사용자가 6번을 선택하기 전까지 반복
    }
}

void returnBook() {
    int studentId;
    printf(">> 도서 반납 <<\n");
    printf("반납할 회원의 학번을 입력하세요: ");
    scanf("%d", &studentId);

    // 해당 학번의 대여 기록 찾기
    Borrow* current = c_head;
    Borrow* matchedBorrows[100];
    int matchedCount = 0;

    while (current) {
        if (current->studentId == studentId) {
            matchedBorrows[matchedCount++] = current;
        }
        current = current->next;
    }

    if (matchedCount == 0) {
        printf("해당 학번의 대여 기록이 없습니다.\n");
        return;
    }

    // 대여된 도서 목록 표시
    printf("\n>> 대여 목록 <<\n");
    for (int i = 0; i < matchedCount; i++) {
        // 도서 정보 찾기
        Book* book = b_head;
        while (book) {
            if (book->bookId == matchedBorrows[i]->bookId) {
                break;
            }
            book = book->next;
        }

        if (book) {
            // 대여일자와 반납일자 포맷팅 (strftime 대신 sprintf 사용)
            char borrowDateStr[11]; // YYYY-MM-DD\0
            char returnDateStr[11]; // YYYY-MM-DD\0

            struct tm* borrow_tm = localtime(&matchedBorrows[i]->borrowDate);
            struct tm* return_tm = localtime(&matchedBorrows[i]->returnDate);

            if (borrow_tm == NULL || return_tm == NULL) {
                printf("날짜 변환에 실패했습니다.\n");
                continue;
            }

            // 수동으로 날짜 문자열 구성
            sprintf(borrowDateStr, "%04d-%02d-%02d",
                    borrow_tm->tm_year + 1900,
                    borrow_tm->tm_mon + 1,
                    borrow_tm->tm_mday);

            sprintf(returnDateStr, "%04d-%02d-%02d",
                    return_tm->tm_year + 1900,
                    return_tm->tm_mon + 1,
                    return_tm->tm_mday);

            printf("도서번호: %d | 도서명: %s | 대여일자: %s | 반납일자: %s\n",
                   book->bookId, book->title, borrowDateStr, returnDateStr);
        }
    }

    // 반납할 도서번호 입력
    int returnBookId;
    printf("\n반납할 도서의 도서번호를 입력하세요: ");
    scanf("%d", &returnBookId);

    // 대여 기록에서 해당 도서 찾기
    Borrow* borrowToRemove = NULL;
    Borrow* prevBorrow = NULL;
    current = c_head;
    while (current) {
        if (current->studentId == studentId && current->bookId == returnBookId) {
            borrowToRemove = current;
            break;
        }
        prevBorrow = current;
        current = current->next;
    }

    if (!borrowToRemove) {
        printf("해당 도서번호의 대여 기록을 찾을 수 없습니다.\n");
        return;
    }

    // 대여 기록 삭제
    if (prevBorrow) {
        prevBorrow->next = borrowToRemove->next;
    } else {
        c_head = borrowToRemove->next;
    }
    free(borrowToRemove);

    // 도서의 대여 가능 여부를 'Y'로 변경
    Book* bookToUpdate = b_head;
    while (bookToUpdate) {
        if (bookToUpdate->bookId == returnBookId) {
            bookToUpdate->available = 'Y';
            break;
        }
        bookToUpdate = bookToUpdate->next;
    }

    // 파일 저장
    saveBooksToFile("book.txt");
    saveBorrow("borrow.txt"); // 대여 목록 저장

    printf("도서가 성공적으로 반납되었습니다.\n");
}


// 프로그램 시작
void startProgram() {
    int choice;
    char role[10];
    int loggedInUser;

    while (1) {
        displayInitialMenu();
        scanf("%d", &choice);

        if (choice == 1) {
            registerMember();
        } else if (choice == 2) {
            if (login(role, &loggedInUser)) {
                if (strcmp(role, "admin") == 0) {
                    int adminChoice;
                    do {
                        displayAdminMenu();
                        scanf("%d", &adminChoice);

                        switch (adminChoice) {
                            case 1:
                                registerBook();
                                break;
                            case 2:
                                deleteBook();
                                break;
                            case 3:
                                borrowBook();
                                break;
                            case 4:
                                returnBook();
                                break;
                            case 5:
                                searchBook(); // 도서 검색
                                break;
                            case 6:
                                viewMemberList(); // 회원 목록 조회
                                break;
                            case 7:
                                printf("관리자 로그아웃\n");
                                break;
                            case 8:
                                printf("프로그램 종료\n");
                                exit(0);
                            default:
                                printf("잘못된 선택입니다.\n");
                        }
                    } while (adminChoice != 7);
                } else if (strcmp(role, "member") == 0) {
                    int memberChoice;
                    do {
                        displayMemberMenu();
                        scanf("%d", &memberChoice);

                        switch (memberChoice) {
                            case 1:
                                searchBook(); // 도서 검색
                                break;
                            case 2:
                                my_borrow_list(loggedInUser); // 내 대여 목록 출력
                                break;
                            case 3:
                                edit_my_info(loggedInUser); // 개인정보 수정 기능
                                break;
                            case 4:
                                if(del_my_info(loggedInUser)) // 회원탈퇴
                                    memberChoice = 5;
                                break;
                            case 5:
                                printf("회원 로그아웃\n");
                                break;
                            case 6:
                                printf("프로그램 종료\n");
                                exit(0);
                            default:
                                printf("잘못된 선택입니다.\n");
                        }
                    } while (memberChoice != 5);
                }
            }
        } else if (choice == 3) {
            printf("프로그램 종료\n");
            break;
        } else {
            printf("잘못된 선택입니다.\n");
        }
    }
}

int main() {
    Client_linkedlist();
    Book_linkedlist();
    Borrow_linkedlist();
    startProgram();
    return 0;
}
