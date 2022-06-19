# interrupts and exceptions handler
# push all registers, call trap_handler(), restore, return.
#
.section k_eentry
.globl trap_entry
.align 4
trap_entry:
        // make room to save registers.
        addi.d $sp, $sp, -256

        // save the registers.
        st.d $ra, $sp, 0
        st.d $tp, $sp, 8
        st.d $sp, $sp, 16
        st.d $a0, $sp, 24
        st.d $a1, $sp, 32
        st.d $a2, $sp, 40
        st.d $a3, $sp, 48
        st.d $a4, $sp, 56
        st.d $a5, $sp, 64
        st.d $a6, $sp, 72
        st.d $a7, $sp, 80
        st.d $t0, $sp, 88
        st.d $t1, $sp, 96
        st.d $t2, $sp, 104
        st.d $t3, $sp, 112
        st.d $t4, $sp, 120
        st.d $t5, $sp, 128
        st.d $t6, $sp, 136
        st.d $t7, $sp, 144
        st.d $t8, $sp, 152
        st.d $r21, $sp,160
        st.d $fp, $sp, 168
        st.d $s0, $sp, 176
        st.d $s1, $sp, 184
        st.d $s2, $sp, 192
        st.d $s3, $sp, 200
        st.d $s4, $sp, 208
        st.d $s5, $sp, 216
        st.d $s6, $sp, 224
        st.d $s7, $sp, 232
        st.d $s8, $sp, 240

	    // call the C trap handler in trap.c
        bl trap_handler

        // restore register
        ld.d $ra, $sp, 0
        ld.d $tp, $sp, 8
        ld.d $sp, $sp, 16
        ld.d $a0, $sp, 24
        ld.d $a1, $sp, 32
        ld.d $a2, $sp, 40
        ld.d $a3, $sp, 48
        ld.d $a4, $sp, 56
        ld.d $a5, $sp, 64
        ld.d $a6, $sp, 72
        ld.d $a7, $sp, 80
        ld.d $t0, $sp, 88
        ld.d $t1, $sp, 96
        ld.d $t2, $sp, 104
        ld.d $t3, $sp, 112
        ld.d $t4, $sp, 120
        ld.d $t5, $sp, 128
        ld.d $t6, $sp, 136
        ld.d $t7, $sp, 144
        ld.d $t8, $sp, 152
        ld.d $r21, $sp,160
        ld.d $fp, $sp, 168
        ld.d $s0, $sp, 176
        ld.d $s1, $sp, 184
        ld.d $s2, $sp, 192
        ld.d $s3, $sp, 200
        ld.d $s4, $sp, 208
        ld.d $s5, $sp, 216
        ld.d $s6, $sp, 224
        ld.d $s7, $sp, 232
        ld.d $s8, $sp, 240

        addi.d $sp, $sp, 256

        // exception return
        ertn
