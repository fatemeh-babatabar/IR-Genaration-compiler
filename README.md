
# example 
   
   teslang code :
        
       num sum(num a , num b){
           return a+b+3;
       }
       num main(){
           num c;
           c=sum(1 , 2);
           return 0;
       }
 
 
  IR code :
  
       proc sum
	       add r0, r0, r1
	    mov r2 ,3
	    add r0, r0, r2
	    ret
      proc main
           mov r3 ,1
           mov r4 ,2
           call sum ,r3 ,r4
           mov r1 ,r3
           mov r5 ,0
           mov r0 , r5
           ret
