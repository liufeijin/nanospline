#pragma once

#include <Eigen/Core>

#include <nanospline/CurveBase.h>

namespace nanospline {

template<typename _Scalar, int _dim, int _degree, bool _generic>
class BezierBase : public CurveBase<_Scalar, _dim> {
    public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static_assert(_dim > 0, "Dimension must be positive.");
        static_assert(_degree>=0 || _generic,
                "Invalid degree for non-generic Bezier setting");
        using Base = CurveBase<_Scalar, _dim>;
        using Scalar = _Scalar;
        using Point = Eigen::Matrix<Scalar, 1, _dim>;
        using ControlPoints = Eigen::Matrix<Scalar, _generic?Eigen::Dynamic:_degree+1, _dim>;
        using BlossomVector = Eigen::Matrix<Scalar, _generic?Eigen::Dynamic:_degree, 1>;

    public:
        virtual ~BezierBase()=default;
        virtual Point evaluate(Scalar t) const override =0;
        virtual Scalar inverse_evaluate(const Point& p) const override =0;
        virtual Point evaluate_derivative(Scalar t) const override =0;
        virtual Point evaluate_2nd_derivative(Scalar t) const override =0;

        virtual bool in_domain(Scalar t) const override {
            constexpr Scalar eps = std::numeric_limits<Scalar>::epsilon();
            const Scalar t_min = 0.;
            const Scalar t_max = 1.;
            return (t >= t_min - eps) && (t <= t_max + eps);
        }
        virtual Scalar approximate_inverse_evaluate(const Point& p,
                const Scalar lower=0.0,
                const Scalar upper=1.0,
                const int level=3) const override {
            const int num_samples = 2 * (get_degree()+1);

            return Base::approximate_inverse_evaluate(
                    p, num_samples, lower, upper, level);
        }

        virtual std::vector<Scalar> compute_inflections(
                const Scalar lower,
                const Scalar upper) const override {
            throw not_implemented_error(
                    "Inflection computation is not support for this curve type");
        }

        virtual std::vector<Scalar> reduce_turning_angle(
                const Scalar lower,
                const Scalar upper) const override {
            throw not_implemented_error(
                    "Turning angle reduction is not support for this curve type");
        }

        virtual std::vector<Scalar> compute_singularities(
                const Scalar lower=0.0,
                const Scalar upper=1.0) const override {
            throw not_implemented_error(
                    "Compute singularity is not support for this curve type");
        }

    public:
        const ControlPoints& get_control_points() const {
            return m_control_points;
        }

        template<typename Derived>
        void set_control_points(const Eigen::PlainObjectBase<Derived>& ctrl_pts) {
            m_control_points = ctrl_pts;
        }

        template<typename Derived>
        void set_control_points(Eigen::PlainObjectBase<Derived>&& ctrl_pts) {
            m_control_points.swap(ctrl_pts);
        }

        int get_degree() const {
            return _generic ? static_cast<int>(m_control_points.rows())-1 : _degree;
        }

        Scalar get_domain_lower_bound() const override {
            return 0.0;
        }

        Scalar get_domain_upper_bound() const override {
            return 1.0;
        }


        virtual void write(std::ostream &out) const override {
            out << "c:\n" << m_control_points << "\n";
        }

    protected:
        ControlPoints m_control_points;
};

}
