#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


void sayHi(char name[]){
    printf("hello %s\n",name);
};

double cube(double num){
    double result = num * num * num;
    return result;
};

//structs
struct student{
    char name[50];
    char major[50];
    int age;
    double gpa;
};

int main(){ //C STARTS AT MAIN
    printf("hello world\n");
    int a = 5;
    const int NUM = 7;
    double b = 3.14;
    char c = 'A';// single quotation marks and single character
    char phrase[] = "Hello, C!"; // double quotation marks for strings
    printf("%s\n",phrase);
    printf("%d\n",50);//digit
    //if %lf it is long float
    printf("number is %f\n",4.5768);//float
    printf("%c\n",c);//character
    printf("%f\n",pow(2,3));
    printf("%f\n",ceil(2.3));
    printf("%f\n",floor(2.3));
    printf("%f\n",sqrt(2.3));
    printf("hello\"world\n");//backslash for dereferencing
    //user input
    int age;
    printf("enter age : \n");
    // scanf("%d",&age); //should use & before var
    //for taking user input
    // printf("input : %d\n",age);
    char name[20];
    printf("enter name : \n");
    //scanf("%s", name);
            //for taking user input and it only takes till a space comes in input
            //so to get everything written in string use fgets
    // fgets(name,20,stdin); 
            //it stores including space buttttt
            //downside is it creates a new line just after %s
    // printf("name is : %s hello\n",name);
    // printf("%c",name[0]);
            //we  can also print individual input of string
    int luckyNum[] = {1,2,3,4,5,6,7,8};
    luckyNum[1] = 200;
    int set[10]; //intialisation without values
    sayHi("mike");
    printf("%f\n", cube(8)); // if returned fun then keep it in printf
    char grade = 'B';
    switch(grade){
    case 'A' :
        printf("you did great\n");
        break;
    case 'B' :
        printf("u did alright\n");
        break;
    default :
        printf("invalid");
    };

    struct student student1 ;
    student1.age = 22;
    student1.gpa =3.2;
    strcpy(student1.name, "Jim");//because C does NOT allow assigning arrays directly Instead you COPY characters one by one
    strcpy(student1.major,"buisiness");
    //memoryadresses
    int a2=2;
    double b2=23.5;
    char c2 = 'r';
    printf(" %p\n %p\n %p\n",&a2,&b2,&c2);//%p==pointer(shows address)

    //pointers : justlike another type of data
    double cgpa = 3.5;
    char grading = 'd';
    int aging =30;
    printf("agings's memory address: %p\n",&aging); //this is physical memory address
    int* p = &aging; // compuldory we have to give the addresss 
    double* q =&cgpa; //type* == address of var of typr "type"
    char* r = &grading;
    printf("%p\n%p\n%p\n",p,q,r);
    //dereferencing a pointer == we have p which has address of aging and if we dereference p it is value at p i.e value at address at which p is pointing to so value at aging
    printf("%d\n%f\n%c\n",*p,*q,*r);
    printf("or---------------------------------------\n");
    printf("%d\n%f\n%c\n",*&aging,*&cgpa,*&grading);
    //writing files
    FILE* fp = fopen("employees.txt","w");//give path
    //making ptr into a physical file
    //fp has address of file 
    fprintf(fp, "jim, salesmen\npam, receptionist");
    fclose(fp);//always close the file if opened
    FILE* fp2 = fopen("employee.txt","a");//append
    fprintf(fp2, "jimi, banker\npamur, caterer");
    fclose(fp2);
    char line[255];
    FILE* fp3 = fopen("empty.txt","r");
    fgets(line, 255, fp3);//firstline
    fgets(line, 255, fp3);//secondline
    printf("%s", line);//will print secondline as it is called at last
    fclose(fp3);
    return 0;
};
