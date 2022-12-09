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
   =================================================