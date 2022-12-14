//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// Automatically generated file, do not edit!
//===----------------------------------------------------------------------===//


#ifndef _HVX_HEXAGON_PROTOS_H_
#define _HVX_HEXAGON_PROTOS_H_ 1

#ifdef __HVX__
#if __HVX_LENGTH__ == 128
#define __BUILTIN_VECTOR_WRAP(a) a ## _128B
#else
#define __BUILTIN_VECTOR_WRAP(a) a
#endif

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Rd32=vextract(Vu32,Rs32)
   C Intrinsic Prototype: Word32 Q6_R_vextract_VR(HVX_Vector Vu, Word32 Rs)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_R_vextract_VR(Vu,Rs) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_extractw)(Vu,Rs)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32=hi(Vss32)
   C Intrinsic Prototype: HVX_Vector Q6_V_hi_W(HVX_VectorPair Vss)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_V_hi_W(Vss) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_hi)(Vss)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32=lo(Vss32)
   C Intrinsic Prototype: HVX_Vector Q6_V_lo_W(HVX_VectorPair Vss)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_V_lo_W(Vss) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_lo)(Vss)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32=vsplat(Rt32)
   C Intrinsic Prototype: HVX_Vector Q6_V_vsplat_R(Word32 Rt)
   Instruction Type:      CVI_VX_LATE
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_V_vsplat_R(Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_lvsplatw)(Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=and(Qs4,Qt4)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_and_QQ(HVX_VectorPred Qs, HVX_VectorPred Qt)
   Instruction Type:      CVI_VA_DV
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_and_QQ(Qs,Qt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_pred_and)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qs),-1),__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qt),-1))),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=and(Qs4,!Qt4)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_and_QQn(HVX_VectorPred Qs, HVX_VectorPred Qt)
   Instruction Type:      CVI_VA_DV
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_and_QQn(Qs,Qt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_pred_and_n)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qs),-1),__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qt),-1))),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=not(Qs4)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_not_Q(HVX_VectorPred Qs)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_not_Q(Qs) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_pred_not)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qs),-1))),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=or(Qs4,Qt4)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_or_QQ(HVX_VectorPred Qs, HVX_VectorPred Qt)
   Instruction Type:      CVI_VA_DV
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_or_QQ(Qs,Qt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_pred_or)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qs),-1),__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qt),-1))),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=or(Qs4,!Qt4)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_or_QQn(HVX_VectorPred Qs, HVX_VectorPred Qt)
   Instruction Type:      CVI_VA_DV
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_or_QQn(Qs,Qt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_pred_or_n)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qs),-1),__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qt),-1))),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=vsetq(Rt32)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vsetq_R(Word32 Rt)
   Instruction Type:      CVI_VP
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vsetq_R(Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_pred_scalar2)(Rt)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=xor(Qs4,Qt4)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_xor_QQ(HVX_VectorPred Qs, HVX_VectorPred Qt)
   Instruction Type:      CVI_VA_DV
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_xor_QQ(Qs,Qt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_pred_xor)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qs),-1),__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qt),-1))),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       if (!Qv4) vmem(Rt32+#s4)=Vs32
   C Intrinsic Prototype: void Q6_vmem_QnRIV(HVX_VectorPred Qv, HVX_Vector* Rt, HVX_Vector Vs)
   Instruction Type:      CVI_VM_ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_vmem_QnRIV(Qv,Rt,Vs) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vS32b_nqpred_ai)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qv),-1),Rt,Vs)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       if (!Qv4) vmem(Rt32+#s4):nt=Vs32
   C Intrinsic Prototype: void Q6_vmem_QnRIV_nt(HVX_VectorPred Qv, HVX_Vector* Rt, HVX_Vector Vs)
   Instruction Type:      CVI_VM_ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_vmem_QnRIV_nt(Qv,Rt,Vs) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vS32b_nt_nqpred_ai)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qv),-1),Rt,Vs)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       if (Qv4) vmem(Rt32+#s4):nt=Vs32
   C Intrinsic Prototype: void Q6_vmem_QRIV_nt(HVX_VectorPred Qv, HVX_Vector* Rt, HVX_Vector Vs)
   Instruction Type:      CVI_VM_ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_vmem_QRIV_nt(Qv,Rt,Vs) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vS32b_nt_qpred_ai)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qv),-1),Rt,Vs)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       if (Qv4) vmem(Rt32+#s4)=Vs32
   C Intrinsic Prototype: void Q6_vmem_QRIV(HVX_VectorPred Qv, HVX_Vector* Rt, HVX_Vector Vs)
   Instruction Type:      CVI_VM_ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_vmem_QRIV(Qv,Rt,Vs) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vS32b_qpred_ai)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qv),-1),Rt,Vs)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.uh=vabsdiff(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_Vector Q6_Vuh_vabsdiff_VhVh(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VX
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vuh_vabsdiff_VhVh(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsdiffh)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.ub=vabsdiff(Vu32.ub,Vv32.ub)
   C Intrinsic Prototype: HVX_Vector Q6_Vub_vabsdiff_VubVub(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VX
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vub_vabsdiff_VubVub(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsdiffub)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.uh=vabsdiff(Vu32.uh,Vv32.uh)
   C Intrinsic Prototype: HVX_Vector Q6_Vuh_vabsdiff_VuhVuh(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VX
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vuh_vabsdiff_VuhVuh(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsdiffuh)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.uw=vabsdiff(Vu32.w,Vv32.w)
   C Intrinsic Prototype: HVX_Vector Q6_Vuw_vabsdiff_VwVw(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VX
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vuw_vabsdiff_VwVw(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsdiffw)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vabs(Vu32.h)
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vabs_Vh(HVX_Vector Vu)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vabs_Vh(Vu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsh)(Vu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vabs(Vu32.h):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vabs_Vh_sat(HVX_Vector Vu)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vabs_Vh_sat(Vu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsh_sat)(Vu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vabs(Vu32.w)
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vabs_Vw(HVX_Vector Vu)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vw_vabs_Vw(Vu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsw)(Vu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vabs(Vu32.w):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vabs_Vw_sat(HVX_Vector Vu)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vw_vabs_Vw_sat(Vu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsw_sat)(Vu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.b=vadd(Vu32.b,Vv32.b)
   C Intrinsic Prototype: HVX_Vector Q6_Vb_vadd_VbVb(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vb_vadd_VbVb(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddb)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32.b=vadd(Vuu32.b,Vvv32.b)
   C Intrinsic Prototype: HVX_VectorPair Q6_Wb_vadd_WbWb(HVX_VectorPair Vuu, HVX_VectorPair Vvv)
   Instruction Type:      CVI_VA_DV
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Wb_vadd_WbWb(Vuu,Vvv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddb_dv)(Vuu,Vvv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       if (!Qv4) Vx32.b+=Vu32.b
   C Intrinsic Prototype: HVX_Vector Q6_Vb_condacc_QnVbVb(HVX_VectorPred Qv, HVX_Vector Vx, HVX_Vector Vu)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vb_condacc_QnVbVb(Qv,Vx,Vu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddbnq)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qv),-1),Vx,Vu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       if (Qv4) Vx32.b+=Vu32.b
   C Intrinsic Prototype: HVX_Vector Q6_Vb_condacc_QVbVb(HVX_VectorPred Qv, HVX_Vector Vx, HVX_Vector Vu)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vb_condacc_QVbVb(Qv,Vx,Vu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddbq)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qv),-1),Vx,Vu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vadd(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vadd_VhVh(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vadd_VhVh(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddh)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32.h=vadd(Vuu32.h,Vvv32.h)
   C Intrinsic Prototype: HVX_VectorPair Q6_Wh_vadd_WhWh(HVX_VectorPair Vuu, HVX_VectorPair Vvv)
   Instruction Type:      CVI_VA_DV
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Wh_vadd_WhWh(Vuu,Vvv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddh_dv)(Vuu,Vvv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       if (!Qv4) Vx32.h+=Vu32.h
   C Intrinsic Prototype: HVX_Vector Q6_Vh_condacc_QnVhVh(HVX_VectorPred Qv, HVX_Vector Vx, HVX_Vector Vu)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_condacc_QnVhVh(Qv,Vx,Vu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddhnq)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qv),-1),Vx,Vu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       if (Qv4) Vx32.h+=Vu32.h
   C Intrinsic Prototype: HVX_Vector Q6_Vh_condacc_QVhVh(HVX_VectorPred Qv, HVX_Vector Vx, HVX_Vector Vu)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_condacc_QVhVh(Qv,Vx,Vu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddhq)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qv),-1),Vx,Vu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vadd(Vu32.h,Vv32.h):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vadd_VhVh_sat(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vadd_VhVh_sat(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddhsat)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32.h=vadd(Vuu32.h,Vvv32.h):sat
   C Intrinsic Prototype: HVX_VectorPair Q6_Wh_vadd_WhWh_sat(HVX_VectorPair Vuu, HVX_VectorPair Vvv)
   Instruction Type:      CVI_VA_DV
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Wh_vadd_WhWh_sat(Vuu,Vvv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddhsat_dv)(Vuu,Vvv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32.w=vadd(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_VectorPair Q6_Ww_vadd_VhVh(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VX_DV
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Ww_vadd_VhVh(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddhw)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32.h=vadd(Vu32.ub,Vv32.ub)
   C Intrinsic Prototype: HVX_VectorPair Q6_Wh_vadd_VubVub(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VX_DV
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Wh_vadd_VubVub(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddubh)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.ub=vadd(Vu32.ub,Vv32.ub):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vub_vadd_VubVub_sat(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vub_vadd_VubVub_sat(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddubsat)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32.ub=vadd(Vuu32.ub,Vvv32.ub):sat
   C Intrinsic Prototype: HVX_VectorPair Q6_Wub_vadd_WubWub_sat(HVX_VectorPair Vuu, HVX_VectorPair Vvv)
   Instruction Type:      CVI_VA_DV
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Wub_vadd_WubWub_sat(Vuu,Vvv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddubsat_dv)(Vuu,Vvv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.uh=vadd(Vu32.uh,Vv32.uh):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vuh_vadd_VuhVuh_sat(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vuh_vadd_VuhVuh_sat(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vadduhsat)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32.uh=vadd(Vuu32.uh,Vvv32.uh):sat
   C Intrinsic Prototype: HVX_VectorPair Q6_Wuh_vadd_WuhWuh_sat(HVX_VectorPair Vuu, HVX_VectorPair Vvv)
   Instruction Type:      CVI_VA_DV
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Wuh_vadd_WuhWuh_sat(Vuu,Vvv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vadduhsat_dv)(Vuu,Vvv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32.w=vadd(Vu32.uh,Vv32.uh)
   C Intrinsic Prototype: HVX_VectorPair Q6_Ww_vadd_VuhVuh(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VX_DV
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Ww_vadd_VuhVuh(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vadduhw)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vadd(Vu32.w,Vv32.w)
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vadd_VwVw(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vw_vadd_VwVw(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddw)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32.w=vadd(Vuu32.w,Vvv32.w)
   C Intrinsic Prototype: HVX_VectorPair Q6_Ww_vadd_WwWw(HVX_VectorPair Vuu, HVX_VectorPair Vvv)
   Instruction Type:      CVI_VA_DV
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Ww_vadd_WwWw(Vuu,Vvv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddw_dv)(Vuu,Vvv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       if (!Qv4) Vx32.w+=Vu32.w
   C Intrinsic Prototype: HVX_Vector Q6_Vw_condacc_QnVwVw(HVX_VectorPred Qv, HVX_Vector Vx, HVX_Vector Vu)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vw_condacc_QnVwVw(Qv,Vx,Vu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddwnq)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qv),-1),Vx,Vu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       if (Qv4) Vx32.w+=Vu32.w
   C Intrinsic Prototype: HVX_Vector Q6_Vw_condacc_QVwVw(HVX_VectorPred Qv, HVX_Vector Vx, HVX_Vector Vu)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vw_condacc_QVwVw(Qv,Vx,Vu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddwq)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qv),-1),Vx,Vu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vadd(Vu32.w,Vv32.w):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vadd_VwVw_sat(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vw_vadd_VwVw_sat(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddwsat)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32.w=vadd(Vuu32.w,Vvv32.w):sat
   C Intrinsic Prototype: HVX_VectorPair Q6_Ww_vadd_WwWw_sat(HVX_VectorPair Vuu, HVX_VectorPair Vvv)
   Instruction Type:      CVI_VA_DV
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Ww_vadd_WwWw_sat(Vuu,Vvv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddwsat_dv)(Vuu,Vvv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32=valign(Vu32,Vv32,Rt8)
   C Intrinsic Prototype: HVX_Vector Q6_V_valign_VVR(HVX_Vector Vu, HVX_Vector Vv, Word32 Rt)
   Instruction Type:      CVI_VP
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_V_valign_VVR(Vu,Vv,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_valignb)(Vu,Vv,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32=valign(Vu32,Vv32,#u3)
   C Intrinsic Prototype: HVX_Vector Q6_V_valign_VVI(HVX_Vector Vu, HVX_Vector Vv, Word32 Iu3)
   Instruction Type:      CVI_VP
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_V_valign_VVI(Vu,Vv,Iu3) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_valignbi)(Vu,Vv,Iu3)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32=vand(Vu32,Vv32)
   C Intrinsic Prototype: HVX_Vector Q6_V_vand_VV(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_V_vand_VV(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vand)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32=vand(Qu4,Rt32)
   C Intrinsic Prototype: HVX_Vector Q6_V_vand_QR(HVX_VectorPred Qu, Word32 Rt)
   Instruction Type:      CVI_VX_LATE
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_V_vand_QR(Qu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qu),-1),Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vx32|=vand(Qu4,Rt32)
   C Intrinsic Prototype: HVX_Vector Q6_V_vandor_VQR(HVX_Vector Vx, HVX_VectorPred Qu, Word32 Rt)
   Instruction Type:      CVI_VX_LATE
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_V_vandor_VQR(Vx,Qu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt_acc)(Vx,__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qu),-1),Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=vand(Vu32,Rt32)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vand_VR(HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VX_LATE
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Q_vand_VR(Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)(Vu,Rt)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4|=vand(Vu32,Rt32)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vandor_QVR(HVX_VectorPred Qx, HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VX_LATE
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Q_vandor_QVR(Qx,Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt_acc)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Rt)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vasl(Vu32.h,Rt32)
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vasl_VhR(HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vasl_VhR(Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaslh)(Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vasl(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vasl_VhVh(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vasl_VhVh(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaslhv)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vasl(Vu32.w,Rt32)
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vasl_VwR(HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vw_vasl_VwR(Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaslw)(Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vx32.w+=vasl(Vu32.w,Rt32)
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vaslacc_VwVwR(HVX_Vector Vx, HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vw_vaslacc_VwVwR(Vx,Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaslw_acc)(Vx,Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vasl(Vu32.w,Vv32.w)
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vasl_VwVw(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vw_vasl_VwVw(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaslwv)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vasr(Vu32.h,Rt32)
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vasr_VhR(HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vasr_VhR(Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrh)(Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.b=vasr(Vu32.h,Vv32.h,Rt8):rnd:sat
   C Intrinsic Prototype: HVX_Vector Q6_Vb_vasr_VhVhR_rnd_sat(HVX_Vector Vu, HVX_Vector Vv, Word32 Rt)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vb_vasr_VhVhR_rnd_sat(Vu,Vv,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrhbrndsat)(Vu,Vv,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.ub=vasr(Vu32.h,Vv32.h,Rt8):rnd:sat
   C Intrinsic Prototype: HVX_Vector Q6_Vub_vasr_VhVhR_rnd_sat(HVX_Vector Vu, HVX_Vector Vv, Word32 Rt)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vub_vasr_VhVhR_rnd_sat(Vu,Vv,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrhubrndsat)(Vu,Vv,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.ub=vasr(Vu32.h,Vv32.h,Rt8):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vub_vasr_VhVhR_sat(HVX_Vector Vu, HVX_Vector Vv, Word32 Rt)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vub_vasr_VhVhR_sat(Vu,Vv,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrhubsat)(Vu,Vv,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vasr(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vasr_VhVh(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vasr_VhVh(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrhv)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vasr(Vu32.w,Rt32)
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vasr_VwR(HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vw_vasr_VwR(Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrw)(Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vx32.w+=vasr(Vu32.w,Rt32)
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vasracc_VwVwR(HVX_Vector Vx, HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vw_vasracc_VwVwR(Vx,Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrw_acc)(Vx,Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vasr(Vu32.w,Vv32.w,Rt8)
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vasr_VwVwR(HVX_Vector Vu, HVX_Vector Vv, Word32 Rt)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vasr_VwVwR(Vu,Vv,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrwh)(Vu,Vv,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vasr(Vu32.w,Vv32.w,Rt8):rnd:sat
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vasr_VwVwR_rnd_sat(HVX_Vector Vu, HVX_Vector Vv, Word32 Rt)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vasr_VwVwR_rnd_sat(Vu,Vv,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrwhrndsat)(Vu,Vv,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vasr(Vu32.w,Vv32.w,Rt8):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vasr_VwVwR_sat(HVX_Vector Vu, HVX_Vector Vv, Word32 Rt)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vasr_VwVwR_sat(Vu,Vv,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrwhsat)(Vu,Vv,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.uh=vasr(Vu32.w,Vv32.w,Rt8):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vuh_vasr_VwVwR_sat(HVX_Vector Vu, HVX_Vector Vv, Word32 Rt)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vuh_vasr_VwVwR_sat(Vu,Vv,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrwuhsat)(Vu,Vv,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vasr(Vu32.w,Vv32.w)
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vasr_VwVw(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vw_vasr_VwVw(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrwv)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32=Vu32
   C Intrinsic Prototype: HVX_Vector Q6_V_equals_V(HVX_Vector Vu)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_V_equals_V(Vu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vassign)(Vu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32=Vuu32
   C Intrinsic Prototype: HVX_VectorPair Q6_W_equals_W(HVX_VectorPair Vuu)
   Instruction Type:      CVI_VA_DV
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_W_equals_W(Vuu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vassignp)(Vuu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vavg(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vavg_VhVh(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vavg_VhVh(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavgh)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vavg(Vu32.h,Vv32.h):rnd
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vavg_VhVh_rnd(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vavg_VhVh_rnd(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavghrnd)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.ub=vavg(Vu32.ub,Vv32.ub)
   C Intrinsic Prototype: HVX_Vector Q6_Vub_vavg_VubVub(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vub_vavg_VubVub(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavgub)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.ub=vavg(Vu32.ub,Vv32.ub):rnd
   C Intrinsic Prototype: HVX_Vector Q6_Vub_vavg_VubVub_rnd(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vub_vavg_VubVub_rnd(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavgubrnd)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.uh=vavg(Vu32.uh,Vv32.uh)
   C Intrinsic Prototype: HVX_Vector Q6_Vuh_vavg_VuhVuh(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vuh_vavg_VuhVuh(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavguh)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.uh=vavg(Vu32.uh,Vv32.uh):rnd
   C Intrinsic Prototype: HVX_Vector Q6_Vuh_vavg_VuhVuh_rnd(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vuh_vavg_VuhVuh_rnd(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavguhrnd)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vavg(Vu32.w,Vv32.w)
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vavg_VwVw(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vw_vavg_VwVw(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavgw)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vavg(Vu32.w,Vv32.w):rnd
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vavg_VwVw_rnd(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vw_vavg_VwVw_rnd(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavgwrnd)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.uh=vcl0(Vu32.uh)
   C Intrinsic Prototype: HVX_Vector Q6_Vuh_vcl0_Vuh(HVX_Vector Vu)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vuh_vcl0_Vuh(Vu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vcl0h)(Vu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.uw=vcl0(Vu32.uw)
   C Intrinsic Prototype: HVX_Vector Q6_Vuw_vcl0_Vuw(HVX_Vector Vu)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vuw_vcl0_Vuw(Vu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vcl0w)(Vu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32=vcombine(Vu32,Vv32)
   C Intrinsic Prototype: HVX_VectorPair Q6_W_vcombine_VV(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA_DV
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_W_vcombine_VV(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vcombine)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32=#0
   C Intrinsic Prototype: HVX_Vector Q6_V_vzero()
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_V_vzero() __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vd0)()
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.b=vdeal(Vu32.b)
   C Intrinsic Prototype: HVX_Vector Q6_Vb_vdeal_Vb(HVX_Vector Vu)
   Instruction Type:      CVI_VP
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vb_vdeal_Vb(Vu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdealb)(Vu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.b=vdeale(Vu32.b,Vv32.b)
   C Intrinsic Prototype: HVX_Vector Q6_Vb_vdeale_VbVb(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VP
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vb_vdeale_VbVb(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdealb4w)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vdeal(Vu32.h)
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vdeal_Vh(HVX_Vector Vu)
   Instruction Type:      CVI_VP
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vdeal_Vh(Vu) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdealh)(Vu)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32=vdeal(Vu32,Vv32,Rt8)
   C Intrinsic Prototype: HVX_VectorPair Q6_W_vdeal_VVR(HVX_Vector Vu, HVX_Vector Vv, Word32 Rt)
   Instruction Type:      CVI_VP_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_W_vdeal_VVR(Vu,Vv,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdealvdd)(Vu,Vv,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32=vdelta(Vu32,Vv32)
   C Intrinsic Prototype: HVX_Vector Q6_V_vdelta_VV(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VP
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_V_vdelta_VV(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdelta)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vdmpy(Vu32.ub,Rt32.b)
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vdmpy_VubRb(HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VX
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vh_vdmpy_VubRb(Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpybus)(Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vx32.h+=vdmpy(Vu32.ub,Rt32.b)
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vdmpyacc_VhVubRb(HVX_Vector Vx, HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VX
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vh_vdmpyacc_VhVubRb(Vx,Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpybus_acc)(Vx,Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32.h=vdmpy(Vuu32.ub,Rt32.b)
   C Intrinsic Prototype: HVX_VectorPair Q6_Wh_vdmpy_WubRb(HVX_VectorPair Vuu, Word32 Rt)
   Instruction Type:      CVI_VX_DV
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Wh_vdmpy_WubRb(Vuu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpybus_dv)(Vuu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vxx32.h+=vdmpy(Vuu32.ub,Rt32.b)
   C Intrinsic Prototype: HVX_VectorPair Q6_Wh_vdmpyacc_WhWubRb(HVX_VectorPair Vxx, HVX_VectorPair Vuu, Word32 Rt)
   Instruction Type:      CVI_VX_DV
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Wh_vdmpyacc_WhWubRb(Vxx,Vuu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpybus_dv_acc)(Vxx,Vuu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vdmpy(Vu32.h,Rt32.b)
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vdmpy_VhRb(HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VX
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vw_vdmpy_VhRb(Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhb)(Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vx32.w+=vdmpy(Vu32.h,Rt32.b)
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vdmpyacc_VwVhRb(HVX_Vector Vx, HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VX
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vw_vdmpyacc_VwVhRb(Vx,Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhb_acc)(Vx,Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32.w=vdmpy(Vuu32.h,Rt32.b)
   C Intrinsic Prototype: HVX_VectorPair Q6_Ww_vdmpy_WhRb(HVX_VectorPair Vuu, Word32 Rt)
   Instruction Type:      CVI_VX_DV
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Ww_vdmpy_WhRb(Vuu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhb_dv)(Vuu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vxx32.w+=vdmpy(Vuu32.h,Rt32.b)
   C Intrinsic Prototype: HVX_VectorPair Q6_Ww_vdmpyacc_WwWhRb(HVX_VectorPair Vxx, HVX_VectorPair Vuu, Word32 Rt)
   Instruction Type:      CVI_VX_DV
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Ww_vdmpyacc_WwWhRb(Vxx,Vuu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhb_dv_acc)(Vxx,Vuu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vdmpy(Vuu32.h,Rt32.h):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vdmpy_WhRh_sat(HVX_VectorPair Vuu, Word32 Rt)
   Instruction Type:      CVI_VX_DV
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vw_vdmpy_WhRh_sat(Vuu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhisat)(Vuu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vx32.w+=vdmpy(Vuu32.h,Rt32.h):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vdmpyacc_VwWhRh_sat(HVX_Vector Vx, HVX_VectorPair Vuu, Word32 Rt)
   Instruction Type:      CVI_VX_DV
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vw_vdmpyacc_VwWhRh_sat(Vx,Vuu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhisat_acc)(Vx,Vuu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vdmpy(Vu32.h,Rt32.h):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vdmpy_VhRh_sat(HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VX
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vw_vdmpy_VhRh_sat(Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhsat)(Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vx32.w+=vdmpy(Vu32.h,Rt32.h):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vdmpyacc_VwVhRh_sat(HVX_Vector Vx, HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VX
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vw_vdmpyacc_VwVhRh_sat(Vx,Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhsat_acc)(Vx,Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vdmpy(Vuu32.h,Rt32.uh,#1):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vdmpy_WhRuh_sat(HVX_VectorPair Vuu, Word32 Rt)
   Instruction Type:      CVI_VX_DV
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vw_vdmpy_WhRuh_sat(Vuu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhsuisat)(Vuu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vx32.w+=vdmpy(Vuu32.h,Rt32.uh,#1):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vdmpyacc_VwWhRuh_sat(HVX_Vector Vx, HVX_VectorPair Vuu, Word32 Rt)
   Instruction Type:      CVI_VX_DV
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vw_vdmpyacc_VwWhRuh_sat(Vx,Vuu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhsuisat_acc)(Vx,Vuu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vdmpy(Vu32.h,Rt32.uh):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vdmpy_VhRuh_sat(HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VX
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vw_vdmpy_VhRuh_sat(Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhsusat)(Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vx32.w+=vdmpy(Vu32.h,Rt32.uh):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vdmpyacc_VwVhRuh_sat(HVX_Vector Vx, HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VX
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vw_vdmpyacc_VwVhRuh_sat(Vx,Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhsusat_acc)(Vx,Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vdmpy(Vu32.h,Vv32.h):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vdmpy_VhVh_sat(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VX
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vw_vdmpy_VhVh_sat(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhvsat)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vx32.w+=vdmpy(Vu32.h,Vv32.h):sat
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vdmpyacc_VwVhVh_sat(HVX_Vector Vx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VX_DV
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vw_vdmpyacc_VwVhVh_sat(Vx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhvsat_acc)(Vx,Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32.uw=vdsad(Vuu32.uh,Rt32.uh)
   C Intrinsic Prototype: HVX_VectorPair Q6_Wuw_vdsad_WuhRuh(HVX_VectorPair Vuu, Word32 Rt)
   Instruction Type:      CVI_VX_DV
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Wuw_vdsad_WuhRuh(Vuu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdsaduh)(Vuu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vxx32.uw+=vdsad(Vuu32.uh,Rt32.uh)
   C Intrinsic Prototype: HVX_VectorPair Q6_Wuw_vdsadacc_WuwWuhRuh(HVX_VectorPair Vxx, HVX_VectorPair Vuu, Word32 Rt)
   Instruction Type:      CVI_VX_DV
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Wuw_vdsadacc_WuwWuhRuh(Vxx,Vuu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdsaduh_acc)(Vxx,Vuu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=vcmp.eq(Vu32.b,Vv32.b)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_eq_VbVb(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_eq_VbVb(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqb)(Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4&=vcmp.eq(Vu32.b,Vv32.b)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_eqand_QVbVb(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_eqand_QVbVb(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqb_and)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4|=vcmp.eq(Vu32.b,Vv32.b)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_eqor_QVbVb(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_eqor_QVbVb(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqb_or)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4^=vcmp.eq(Vu32.b,Vv32.b)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_eqxacc_QVbVb(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_eqxacc_QVbVb(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqb_xor)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=vcmp.eq(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_eq_VhVh(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_eq_VhVh(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqh)(Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4&=vcmp.eq(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_eqand_QVhVh(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_eqand_QVhVh(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqh_and)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4|=vcmp.eq(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_eqor_QVhVh(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_eqor_QVhVh(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqh_or)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4^=vcmp.eq(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_eqxacc_QVhVh(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_eqxacc_QVhVh(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqh_xor)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=vcmp.eq(Vu32.w,Vv32.w)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_eq_VwVw(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_eq_VwVw(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqw)(Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4&=vcmp.eq(Vu32.w,Vv32.w)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_eqand_QVwVw(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_eqand_QVwVw(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqw_and)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4|=vcmp.eq(Vu32.w,Vv32.w)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_eqor_QVwVw(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_eqor_QVwVw(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqw_or)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4^=vcmp.eq(Vu32.w,Vv32.w)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_eqxacc_QVwVw(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_eqxacc_QVwVw(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqw_xor)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=vcmp.gt(Vu32.b,Vv32.b)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gt_VbVb(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gt_VbVb(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtb)(Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4&=vcmp.gt(Vu32.b,Vv32.b)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtand_QVbVb(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtand_QVbVb(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtb_and)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4|=vcmp.gt(Vu32.b,Vv32.b)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtor_QVbVb(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtor_QVbVb(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtb_or)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4^=vcmp.gt(Vu32.b,Vv32.b)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtxacc_QVbVb(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtxacc_QVbVb(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtb_xor)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=vcmp.gt(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gt_VhVh(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gt_VhVh(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgth)(Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4&=vcmp.gt(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtand_QVhVh(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtand_QVhVh(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgth_and)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4|=vcmp.gt(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtor_QVhVh(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtor_QVhVh(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgth_or)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4^=vcmp.gt(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtxacc_QVhVh(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtxacc_QVhVh(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgth_xor)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=vcmp.gt(Vu32.ub,Vv32.ub)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gt_VubVub(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gt_VubVub(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtub)(Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4&=vcmp.gt(Vu32.ub,Vv32.ub)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtand_QVubVub(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtand_QVubVub(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtub_and)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4|=vcmp.gt(Vu32.ub,Vv32.ub)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtor_QVubVub(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtor_QVubVub(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtub_or)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4^=vcmp.gt(Vu32.ub,Vv32.ub)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtxacc_QVubVub(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtxacc_QVubVub(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtub_xor)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=vcmp.gt(Vu32.uh,Vv32.uh)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gt_VuhVuh(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gt_VuhVuh(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuh)(Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4&=vcmp.gt(Vu32.uh,Vv32.uh)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtand_QVuhVuh(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtand_QVuhVuh(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuh_and)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4|=vcmp.gt(Vu32.uh,Vv32.uh)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtor_QVuhVuh(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtor_QVuhVuh(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuh_or)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4^=vcmp.gt(Vu32.uh,Vv32.uh)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtxacc_QVuhVuh(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtxacc_QVuhVuh(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuh_xor)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=vcmp.gt(Vu32.uw,Vv32.uw)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gt_VuwVuw(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gt_VuwVuw(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuw)(Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4&=vcmp.gt(Vu32.uw,Vv32.uw)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtand_QVuwVuw(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtand_QVuwVuw(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuw_and)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4|=vcmp.gt(Vu32.uw,Vv32.uw)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtor_QVuwVuw(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtor_QVuwVuw(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuw_or)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4^=vcmp.gt(Vu32.uw,Vv32.uw)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtxacc_QVuwVuw(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtxacc_QVuwVuw(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuw_xor)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qd4=vcmp.gt(Vu32.w,Vv32.w)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gt_VwVw(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gt_VwVw(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtw)(Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4&=vcmp.gt(Vu32.w,Vv32.w)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtand_QVwVw(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtand_QVwVw(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtw_and)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4|=vcmp.gt(Vu32.w,Vv32.w)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtor_QVwVw(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtor_QVwVw(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtw_or)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Qx4^=vcmp.gt(Vu32.w,Vv32.w)
   C Intrinsic Prototype: HVX_VectorPred Q6_Q_vcmp_gtxacc_QVwVw(HVX_VectorPred Qx, HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Q_vcmp_gtxacc_QVwVw(Qx,Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)((__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtw_xor)(__BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)((Qx),-1),Vu,Vv)),-1)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vx32.w=vinsert(Rt32)
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vinsert_VwR(HVX_Vector Vx, Word32 Rt)
   Instruction Type:      CVI_VX_LATE
   Execution Slots:       SLOT23
   ========================================================================== */

#define Q6_Vw_vinsert_VwR(Vx,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vinsertwr)(Vx,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32=vlalign(Vu32,Vv32,Rt8)
   C Intrinsic Prototype: HVX_Vector Q6_V_vlalign_VVR(HVX_Vector Vu, HVX_Vector Vv, Word32 Rt)
   Instruction Type:      CVI_VP
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_V_vlalign_VVR(Vu,Vv,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlalignb)(Vu,Vv,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32=vlalign(Vu32,Vv32,#u3)
   C Intrinsic Prototype: HVX_Vector Q6_V_vlalign_VVI(HVX_Vector Vu, HVX_Vector Vv, Word32 Iu3)
   Instruction Type:      CVI_VP
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_V_vlalign_VVI(Vu,Vv,Iu3) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlalignbi)(Vu,Vv,Iu3)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.uh=vlsr(Vu32.uh,Rt32)
   C Intrinsic Prototype: HVX_Vector Q6_Vuh_vlsr_VuhR(HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vuh_vlsr_VuhR(Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlsrh)(Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vlsr(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vlsr_VhVh(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vlsr_VhVh(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlsrhv)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.uw=vlsr(Vu32.uw,Rt32)
   C Intrinsic Prototype: HVX_Vector Q6_Vuw_vlsr_VuwR(HVX_Vector Vu, Word32 Rt)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vuw_vlsr_VuwR(Vu,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlsrw)(Vu,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.w=vlsr(Vu32.w,Vv32.w)
   C Intrinsic Prototype: HVX_Vector Q6_Vw_vlsr_VwVw(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vw_vlsr_VwVw(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlsrwv)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.b=vlut32(Vu32.b,Vv32.b,Rt8)
   C Intrinsic Prototype: HVX_Vector Q6_Vb_vlut32_VbVbR(HVX_Vector Vu, HVX_Vector Vv, Word32 Rt)
   Instruction Type:      CVI_VP
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vb_vlut32_VbVbR(Vu,Vv,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlutvvb)(Vu,Vv,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vx32.b|=vlut32(Vu32.b,Vv32.b,Rt8)
   C Intrinsic Prototype: HVX_Vector Q6_Vb_vlut32or_VbVbVbR(HVX_Vector Vx, HVX_Vector Vu, HVX_Vector Vv, Word32 Rt)
   Instruction Type:      CVI_VP_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vb_vlut32or_VbVbVbR(Vx,Vu,Vv,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlutvvb_oracc)(Vx,Vu,Vv,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vdd32.h=vlut16(Vu32.b,Vv32.h,Rt8)
   C Intrinsic Prototype: HVX_VectorPair Q6_Wh_vlut16_VbVhR(HVX_Vector Vu, HVX_Vector Vv, Word32 Rt)
   Instruction Type:      CVI_VP_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Wh_vlut16_VbVhR(Vu,Vv,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlutvwh)(Vu,Vv,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vxx32.h|=vlut16(Vu32.b,Vv32.h,Rt8)
   C Intrinsic Prototype: HVX_VectorPair Q6_Wh_vlut16or_WhVbVhR(HVX_VectorPair Vxx, HVX_Vector Vu, HVX_Vector Vv, Word32 Rt)
   Instruction Type:      CVI_VP_VS
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Wh_vlut16or_WhVbVhR(Vxx,Vu,Vv,Rt) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlutvwh_oracc)(Vxx,Vu,Vv,Rt)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.h=vmax(Vu32.h,Vv32.h)
   C Intrinsic Prototype: HVX_Vector Q6_Vh_vmax_VhVh(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vh_vmax_VhVh(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmaxh)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.ub=vmax(Vu32.ub,Vv32.ub)
   C Intrinsic Prototype: HVX_Vector Q6_Vub_vmax_VubVub(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   ========================================================================== */

#define Q6_Vub_vmax_VubVub(Vu,Vv) __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmaxub)(Vu,Vv)
#endif /* __HEXAGON_ARCH___ >= 60 */

#if __HVX_ARCH__ >= 60
/* ==========================================================================
   Assembly Syntax:       Vd32.uh=vmax(Vu32.uh,Vv32.uh)
   C Intrinsic Prototype: HVX_Vector Q6_Vuh_vmax_VuhVuh(HVX_Vector Vu, HVX_Vector Vv)
   Instruction Type:      CVI_VA
   Execution Slots:       SLOT0123
   =====================================