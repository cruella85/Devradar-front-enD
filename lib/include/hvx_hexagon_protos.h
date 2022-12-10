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
#endif /* __HEXAGON_ARCH___